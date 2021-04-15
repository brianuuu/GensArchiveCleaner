#include "gensarchivecleaner.h"
#include "ui_gensarchivecleaner.h"

#include "windows.h"

//---------------------------------------------------------------------------
/// Constructor
//---------------------------------------------------------------------------
GensArchiveCleaner::GensArchiveCleaner(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GensArchiveCleaner)
{
    ui->setupUi(this);

    m_settings = new QSettings("brianuuu", "GensArchiveCleaner", this);
    m_path = m_settings->value("DefaultDirectory", QString()).toString();
    m_pathCopy = m_path;

    m_watcher = Q_NULLPTR;

    m_baseParsed = false;
    m_baseModel = new CustomFileSystemModel(this);
    m_baseModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList baseFilter;
    for (int i = 0; i < Base_ERROR; i++)
    {
        baseFilter << "*" + getBaseExtension(static_cast<BaseExtension>(i));
    }
    m_baseModel->setNameFilters(baseFilter);
    m_baseModel->setNameFilterDisables(false);
    connect(m_baseModel, &CustomFileSystemModel::directoryLoaded, this, &GensArchiveCleaner::base_directoryLoaded);

    m_resourceParsed = false;
    m_resourceModel = new CustomFileSystemModel(this);
    m_resourceModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList resourceFilter;
    for (int i = 0; i < Resource_ERROR; i++)
    {
        resourceFilter << "*" + getResourceExtension(static_cast<ResourceExtension>(i));
    }
    m_resourceModel->setNameFilters(resourceFilter);
    m_resourceModel->setNameFilterDisables(false);
    connect(m_resourceModel, &CustomFileSystemModel::directoryLoaded, this, &GensArchiveCleaner::resource_directoryLoaded);
}

//---------------------------------------------------------------------------
/// Destructor
//---------------------------------------------------------------------------
GensArchiveCleaner::~GensArchiveCleaner()
{
    m_settings->setValue("DefaultDirectory", m_path);

    delete ui;
}

//---------------------------------------------------------------------------
/// Choose directory
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_PB_Open_clicked()
{
    QString path = "";
    if (!m_path.isEmpty())
    {
        path = m_path;
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir == Q_NULLPTR) return;
    m_path = dir;
    ui->LE_Folder->setText(dir);

    LoadDirectory();
}

//---------------------------------------------------------------------------
/// Copy selected base files and all it's resources to a new directory
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_PB_Copy_clicked()
{
    QString path = "";
    if (!m_pathCopy.isEmpty())
    {
        path = m_pathCopy;
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir == Q_NULLPTR) return;
    m_pathCopy = dir;

    int success = 0;
    int fail = 0;

    // Copy base files
    QModelIndexList const list = ui->LV_Base->selectionModel()->selectedIndexes();
    for (QModelIndex const& index : list)
    {
        QString const fileName = m_baseModel->fileName(index);
        if (QFile::copy(m_path + "/" + fileName, m_pathCopy + "/" + fileName))
        {
            success++;
        }
        else
        {
            fail++;
        }
    }

    // Copy resources
    QModelIndexList const list2 = ui->LV_Resource->selectionModel()->selectedIndexes();
    for (QModelIndex const& index : list2)
    {
        QString const fileName = m_resourceModel->fileName(index);
        if (QFile::copy(m_path + "/" + fileName, m_pathCopy + "/" + fileName))
        {
            success++;
        }
        else
        {
            fail++;
        }
    }

    QString const copiedString = QString::number(success) + " files copied";
    if (fail > 0)
    {
        QMessageBox::warning(this, "Copy", copiedString + ", " + QString::number(fail) + " failed", QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(this, "Copy", copiedString, QMessageBox::Ok);
    }
}

//---------------------------------------------------------------------------
/// Move selected files to recycle bin
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_PB_DeleteBase_clicked()
{
    QModelIndexList const list = ui->LV_Base->selectionModel()->selectedIndexes();
    if (list.isEmpty()) return;

    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    QString message = "Move selected base files to recycle bin?";
    resBtn = QMessageBox::warning(this, "Delete", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (resBtn == QMessageBox::No) return;

    for (QModelIndex const& index : list)
    {
        QString const fullName = m_path + "/" + m_baseModel->fileName(index);
        MoveToTrash(fullName);
    }
}

//---------------------------------------------------------------------------
/// Move all unused files to recycle bin
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_PB_Clean_clicked()
{
    QStringList list;
    for (ResourceIter iter = m_resources.begin(); iter != m_resources.end(); iter++)
    {
        if (iter.value().m_unused)
        {
            list << iter.key();
        }
    }

    if (list.isEmpty()) return;

    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    QString message = "Move all potentially unused resources to recycle bin?";
    message += "\nWARNING: They might be used by base files not yet supported by this program.";
    resBtn = QMessageBox::warning(this, "Delete", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (resBtn == QMessageBox::No) return;

    for (QString const& file : list)
    {
        QString const fullName = m_path + "/" + file;
        MoveToTrash(fullName);
    }
}

//---------------------------------------------------------------------------
/// Move selected files to recycle bin
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_PB_DeleteResource_clicked()
{
    QModelIndexList const list = ui->LV_Resource->selectionModel()->selectedIndexes();
    if (list.isEmpty()) return;

    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    QString message = "Move selected base files to recycle bin?";
    resBtn = QMessageBox::warning(this, "Delete", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (resBtn == QMessageBox::No) return;

    for (QModelIndex const& index : list)
    {
        QString const fullName = m_path + "/" + m_resourceModel->fileName(index);
        MoveToTrash(fullName);
    }
}

//---------------------------------------------------------------------------
/// Slot called when current directory is renamed/removed
//---------------------------------------------------------------------------
void GensArchiveCleaner::watcher_directoryChanged(const QString &path)
{
    QString dir = ui->LE_Folder->text();
    if (dir.isEmpty()) return;

    QDir testDir(dir);
    if (!testDir.exists())
    {
        ui->LE_Folder->setText("");

        ui->LV_Base->setModel(Q_NULLPTR);
        SetBaseCount(0);
        SetBaseSelected(0);
        SetBaseError(0);
        m_bases.clear();
        m_baseModel->clearList();

        ui->LV_Resource->setModel(Q_NULLPTR);
        SetResourceCount(0);
        SetResourceSelected(0);
        SetResourceUnused(0);
        m_resources.clear();
        m_resourceModel->clearList();

        QMessageBox::critical(this, "Error", "Directory has been removed!", QMessageBox::Ok);
    }
}

//---------------------------------------------------------------------------
/// Slot called when all files in a directory is loaded
//---------------------------------------------------------------------------
void GensArchiveCleaner::base_directoryLoaded(const QString &path)
{
    int count = m_baseModel->rowCount(m_baseModelIndex);
    SetBaseCount(count);

    // Parse files
    m_bases.clear();
    m_baseModel->clearList();
    ui->LV_Base->clearSelection();
    for (int i = 0; i < count; i++)
    {
        QModelIndex index = m_baseModel->index(i, 0, m_baseModelIndex);
        QString const fileName = m_baseModel->fileName(index);
        QString const fullName = m_path + "/" + fileName;

        BaseExtension be = getBaseExtensionEnum(fileName);
        if (be == Base_ERROR) continue;

        //qDebug() << "Reading base file" << fileName;
        Base base(be, GetBaseFileResources(fullName));
        m_bases.insert(fileName, base);
    }

    // If resource is also a base file, add their resources to the root too
    for (Base& base : m_bases)
    {
        QStringList additionalList;
        for (QString const& resource : base.m_resources)
        {
            if (m_bases.contains(resource))
            {
                for (QString const& additionalResource : m_bases[resource].m_resources)
                {
                    additionalList.append(additionalResource);
                }
            }
        }
        base.addResources(additionalList);
        //qDebug() << additionalList;
    }

    // At this point we don't know if resources are missing yet, it's checked with CheckErrorAndUnused()
    m_baseParsed = true;
    if (m_baseParsed && m_resourceParsed)
    {
        CheckErrorAndUnused();
    }

    qDebug() << m_bases.count() << "base files parsed";
}

//---------------------------------------------------------------------------
/// Open a file
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_LV_Base_doubleClicked(const QModelIndex &index)
{
    QString const fullName = m_path + "/" + m_baseModel->fileName(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(fullName));
}

//---------------------------------------------------------------------------
/// Base file selection
//---------------------------------------------------------------------------
void GensArchiveCleaner::base_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList const list = ui->LV_Base->selectionModel()->selectedIndexes();
    SetBaseSelected(list.size());

    // Retrive all resources of this base file
    QSet<QString> selectedResources;
    for (QModelIndex const& index : list)
    {
        QString const fileName = m_baseModel->fileName(index);
        if (m_bases.contains(fileName))
        {
            Base const& base = m_bases[fileName];
            selectedResources.unite(base.m_resources);
        }
    }

    // Select resrouces programatically
    QItemSelectionModel* selectionModel = ui->LV_Resource->selectionModel();
    selectionModel->clearSelection();
    QModelIndex topIndex;
    QModelIndex bottomIndex;
    for (QString const& resource : selectedResources)
    {
        QModelIndex index = m_resourceModel->index(m_path + "/" + resource);
        Q_ASSERT(m_resources.contains(resource) && index.isValid());

        selectionModel->select(index, QItemSelectionModel::Select);
        if (!topIndex.isValid() || index < topIndex) topIndex = index;
        if (!bottomIndex.isValid() || bottomIndex < index) bottomIndex = index;
    }

    // We scroll to bottom then top to have as many resources in view as possible
    ui->LV_Resource->scrollTo(bottomIndex);
    ui->LV_Resource->scrollTo(topIndex);
}

//---------------------------------------------------------------------------
/// Slot called when all files in a directory is loaded
//---------------------------------------------------------------------------
void GensArchiveCleaner::resource_directoryLoaded(const QString &path)
{
    int count = m_resourceModel->rowCount(m_resourceModelIndex);
    SetResourceCount(count);

    // Parse files
    m_resources.clear();
    m_resourceModel->clearList();
    ui->LV_Resource->clearSelection();
    for (int i = 0; i < count; i++)
    {
        QModelIndex index = m_resourceModel->index(i, 0, m_resourceModelIndex);
        QString const fileName = m_resourceModel->fileName(index);

        ResourceExtension re = getResourceExtensionEnum(fileName);
        if (re == Resource_ERROR) continue;

        //qDebug() << "Reading resource" << fileName;
        Resource resource(re);
        m_resources.insert(fileName, resource);
    }

    m_resourceParsed = true;
    if (m_baseParsed && m_resourceParsed)
    {
        CheckErrorAndUnused();
    }

    qDebug() << m_resources.count() << "resources parsed";
}

//---------------------------------------------------------------------------
/// Open a file
//---------------------------------------------------------------------------
void GensArchiveCleaner::on_LV_Resource_doubleClicked(const QModelIndex &index)
{
    QString const fullName = m_path + "/" + m_resourceModel->fileName(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(fullName));
}

//---------------------------------------------------------------------------
/// Resource selection
//---------------------------------------------------------------------------
void GensArchiveCleaner::resource_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList const list = ui->LV_Resource->selectionModel()->selectedIndexes();
    SetResourceSelected(list.size());
}

//---------------------------------------------------------------------------
/// Load all data of current directory
//---------------------------------------------------------------------------
void GensArchiveCleaner::LoadDirectory()
{
    QString dir = ui->LE_Folder->text();
    if (dir.isEmpty()) return;

    QDir testDir(dir);
    if (!testDir.exists())
    {
        QMessageBox::critical(this, "Error", "Invalid directory", QMessageBox::Ok);
        ui->LE_Folder->setText("");
        return;
    }

    // Watch one directory above so we can check if user deleted this the selected directory
    if (m_watcher)
    {
        delete m_watcher;
    }
    testDir.cdUp();
    m_watcher = new QFileSystemWatcher({testDir.path()}, this);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &GensArchiveCleaner::watcher_directoryChanged);

    // Set base model
    if (!ui->LV_Base->model())
    {
        ui->LV_Base->setModel(m_baseModel);
        connect(ui->LV_Base->selectionModel(), &QItemSelectionModel::selectionChanged, this, &GensArchiveCleaner::base_selectionChanged);
    }
    m_baseModelIndex = m_baseModel->setRootPath(dir);
    ui->LV_Base->clearSelection();
    ui->LV_Base->setRootIndex(m_baseModelIndex);
    SetBaseSelected(0);

    // Set resource model
    if (!ui->LV_Resource->model())
    {
        ui->LV_Resource->setModel(m_resourceModel);
        connect(ui->LV_Resource->selectionModel(), &QItemSelectionModel::selectionChanged, this, &GensArchiveCleaner::resource_selectionChanged);
    }
    m_resourceModelIndex = m_resourceModel->setRootPath(dir);
    ui->LV_Resource->clearSelection();
    ui->LV_Resource->setRootIndex(m_resourceModelIndex);
    SetResourceSelected(0);

    qApp->processEvents();

    // NOTE: We can only start parsing files once directoryLoaded() signal is emitted
}

//---------------------------------------------------------------------------
/// Called when both base and resource have finished directoryChanged()
//---------------------------------------------------------------------------
void GensArchiveCleaner::CheckErrorAndUnused()
{
    QMap<QString, QSet<QString>> used;

    // Check if there are any missing resources
    int errorCount = 0;
    for (BaseIter iter = m_bases.begin(); iter != m_bases.end(); iter++)
    {
        Base& base = iter.value();
        base.m_missingResources.clear();
        for (QString const& resource : base.m_resources)
        {
            if (!m_resources.contains(resource))
            {
                if (base.m_missingResources.isEmpty())
                {
                    errorCount++;
                }

                // Move missing resources to the other QSet
                base.m_missingResources.insert(resource);
            }
            else
            {
                used[resource].insert(iter.key());
            }
        }

        // Now we can safely remove
        for (QString const& missingResource : base.m_missingResources)
        {
            base.m_resources.remove(missingResource);
        }

        QModelIndex index = m_baseModel->index(m_path + "/" + iter.key());
        Q_ASSERT(index.isValid());
        if (!base.m_missingResources.isEmpty())
        {
            //qDebug() << iter.key() << "has missing resources:" << base.m_missingResources;
            m_baseModel->setError(index, base.m_missingResources);
        }
        else
        {
            m_baseModel->setNormal(index);
        }
    }
    SetBaseError(errorCount);

    // Check if any resources are POTENTIALLY unused
    int unusedCount = 0;
    for (ResourceIter iter = m_resources.begin(); iter != m_resources.end(); iter++)
    {
        QModelIndex index = m_resourceModel->index(m_path + "/" + iter.key());
        Q_ASSERT(index.isValid());

        if (!used.contains(iter.key()))
        {
            Resource& resource = iter.value();
            resource.m_unused = true;
            unusedCount++;
            m_resourceModel->setUnused(index);
        }
        else
        {
            m_resourceModel->setNormal(index);
        }
    }

    // If resource is also a base file, mark their resources unused too
    for (ResourceIter iter = m_resources.begin(); iter != m_resources.end(); iter++)
    {
        QString const& baseName = iter.key();
        Resource const& baseResource = iter.value();
        if (baseResource.m_unused && m_bases.contains(baseName))
        {
            for (QString const& resource : m_bases[baseName].m_resources)
            {
                // Resource of this base is of course used by this base
                // but we need to check if its only used by this base only
                Q_ASSERT(used.contains(resource));
                if (used[resource].size() == 1)
                {
                    QModelIndex index = m_resourceModel->index(m_path + "/" + resource);
                    Q_ASSERT(index.isValid());
                    Q_ASSERT(m_resources.contains(resource));
                    m_resources[resource].m_unused = true;
                    unusedCount++;
                    m_resourceModel->setUnused(index);
                }
            }

            // Set base file color to unused too
            QModelIndex index = m_baseModel->index(m_path + "/" + baseName);
            Q_ASSERT(index.isValid());
            m_baseModel->setUnused(index);
        }
    }
    SetResourceUnused(unusedCount);

    // Reset these so only when both is true then this function is called again
    m_baseParsed = false;
    m_resourceParsed = false;
}

//---------------------------------------------------------------------------
/// Get list of resource names of a base file
//---------------------------------------------------------------------------
QStringList GensArchiveCleaner::GetBaseFileResources(const QString &fullName)
{
    QStringList list;

    BaseExtension be = getBaseExtensionEnum(fullName);
    switch (be)
    {
    case Base_GTE:
    {
        Glitter::GlitterEffect effect(fullName.toStdString());
        std::vector<std::shared_ptr<Glitter::Particle>> const particles = effect.getParticles();
        for (std::shared_ptr<Glitter::Particle> const& particle : particles)
        {
            list << QString::fromStdString(particle->getMaterial()) + ".gtm";
        }
        break;
    }
    case Base_GTM:
    {
        Glitter::GlitterMaterial material(fullName.toStdString());
        list << QString::fromStdString(material.getTexture()) + ".dds";
        break;
    }
    case Base_MATERIAL:
    {
        Glitter::Material material(fullName.toStdString());
        std::vector<Glitter::Texture*> const textures = material.getTextures();
        for (Glitter::Texture* tex : textures)
        {
            list << QString::fromStdString(tex->getName()) + ".dds";
        }
        break;
    }
    case Base_MODEL:
    {
        Glitter::Model model(fullName.toStdString());
        std::list<std::string> const materialNames = model.getMaterialNames();
        for (std::string const& str : materialNames)
        {
            list << QString::fromStdString(str) + ".material";
        }
        break;
    }
    case Base_ERROR: break;
    }

    //qDebug() << list;
    return list;
}

//---------------------------------------------------------------------------
/// Delete and move file to recycle bin
//---------------------------------------------------------------------------
bool GensArchiveCleaner::MoveToTrash(const QString &file)
{
    QFileInfo fileinfo(file);
    if(!fileinfo.exists())
    {
        return false;
    }

    WCHAR from[MAX_PATH];
    memset(from, 0, sizeof(from));
    int l = fileinfo.absoluteFilePath().toWCharArray(from);
    Q_ASSERT(0 <= l && l < MAX_PATH);
    from[l] = '\0';
    SHFILEOPSTRUCT fileop;
    memset(&fileop, 0, sizeof(fileop));
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation(&fileop);
    if(0 != rv)
    {
        qDebug() << rv << QString::number(rv).toInt(nullptr, 8);
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
/// Set label numbers
//---------------------------------------------------------------------------
void GensArchiveCleaner::SetBaseCount(int count)
{
    ui->L_BaseItem->setText(QString::number(count) + " items,");
}

void GensArchiveCleaner::SetBaseSelected(int count)
{
    ui->L_BaseSelected->setText(QString::number(count) + " selected,");
    ui->PB_Copy->setEnabled(count > 0);
    ui->PB_DeleteBase->setEnabled(count > 0);
}

void GensArchiveCleaner::SetBaseError(int count)
{
    // this is rich text
    if (count == 0)
    {
        ui->L_BaseError->setText("0 errors");
    }
    else
    {
        ui->L_BaseError->setText("<p style=\"color:red;\">" + QString::number(count) + " errors</p>");
    }
}

void GensArchiveCleaner::SetResourceCount(int count)
{
    ui->L_ResourceItem->setText(QString::number(count) + " items,");
}

void GensArchiveCleaner::SetResourceSelected(int count)
{
    ui->L_ResourceSelected->setText(QString::number(count) + " selected,");
    ui->PB_DeleteResource->setEnabled(count > 0);
}

void GensArchiveCleaner::SetResourceUnused(int count)
{
    // this is rich text
    if (count == 0)
    {
        ui->L_ResourceUnused->setText("0 potentially unused");
    }
    else
    {
        ui->L_ResourceUnused->setText("<p style=\"color:coral;\">" + QString::number(count) + " potentially unused</p>");
    }
    ui->PB_Clean->setEnabled(count > 0);
}
