#ifndef GENSARCHIVECLEANER_H
#define GENSARCHIVECLEANER_H

#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QMainWindow>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>

#include "customfilesystemmodel.h"

#include "GlitterLib/GlitterEffect.h"
#include "GlitterLib/GlitterMaterial.h"
#include "GlitterLib/Material.h"
#include "GlitterLib/Model.h"
#include "GlitterLib/TextureOld.h"
#include "GlitterLib/xncpminumum.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GensArchiveCleaner; }
QT_END_NAMESPACE

// TODO: .*-anim, read from .ar

enum BaseExtension
{
    Base_GTE = 0,
    Base_GTM,
    Base_MATERIAL,
    Base_MODEL,
    Base_TEXTURE,
    Base_XNCP,

    Base_ERROR,
};

inline QString getBaseExtension(BaseExtension be)
{
    switch (be)
    {
    case Base_GTE:      return ".gte";
    case Base_GTM:      return ".gtm";
    case Base_MATERIAL: return ".material";
    case Base_MODEL:    return ".model";
    case Base_TEXTURE:  return ".texture";
    case Base_XNCP:     return ".xncp";
    default:            return "";
    }
}

inline BaseExtension getBaseExtensionEnum(QString const& file)
{
    for (int i = 0; i < Base_ERROR; i++)
    {
        BaseExtension be = static_cast<BaseExtension>(i);
        if (file.endsWith(getBaseExtension(be)))
        {
            return be;
        }
    }
    return Base_ERROR;
}

enum ResourceExtension
{
    Resource_DDS = 0,
    Resource_GTM,
    Resource_MATERIAL,
    Resource_TEXTURE,

    Resource_ERROR,
};

inline QString getResourceExtension(ResourceExtension be)
{
    switch (be)
    {
    case Resource_DDS:      return ".dds";
    case Resource_GTM:      return ".gtm";
    case Resource_MATERIAL: return ".material";
    case Resource_TEXTURE:  return ".texture";
    default:                return "";
    }
}

inline ResourceExtension getResourceExtensionEnum(QString const& file)
{
    for (int i = 0; i < Resource_ERROR; i++)
    {
        ResourceExtension re = static_cast<ResourceExtension>(i);
        if (file.endsWith(getResourceExtension(re)))
        {
            return re;
        }
    }
    return Resource_ERROR;
}

class GensArchiveCleaner : public QMainWindow
{
    Q_OBJECT

public:
    GensArchiveCleaner(QWidget *parent = nullptr);
    ~GensArchiveCleaner();

private slots:
    void on_PB_Open_clicked();
    void on_PB_Copy_clicked();
    void on_PB_DeleteBase_clicked();
    void on_PB_Clean_clicked();
    void on_PB_DeleteResource_clicked();

    void watcher_directoryChanged(const QString &path);

    void on_TV_Base_doubleClicked(const QModelIndex &index);
    void base_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void base_directoryLoaded(const QString &path);

    void on_TV_Resource_doubleClicked(const QModelIndex &index);
    void resource_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void resource_directoryLoaded(const QString &path);

private:
    void LoadDirectory();
    void CheckErrorAndUnused();
    bool IsResourceUnused(QString const& baseName);
    QStringList GetBaseFileResources(QString const& fullName);

    bool MoveToTrash(QString const& file);

    void SetBaseCount(int count);
    void SetBaseSelected(int count);
    void SetBaseError(int count);

    void SetResourceCount(int count);
    void SetResourceSelected(int count);
    void SetResourceUnused(int count);

private:
    Ui::GensArchiveCleaner *ui;

    QSettings *m_settings;
    QString m_path;
    QString m_pathCopy;

    QFileSystemWatcher* m_watcher;
    CustomFileSystemModel* m_baseModel;
    QModelIndex m_baseModelIndex;
    CustomFileSystemModel* m_resourceModel;
    QModelIndex m_resourceModelIndex;

    struct Resource
    {
        Resource(){}
        Resource(ResourceExtension type, bool unused = false)
            : m_type(type)
            , m_unused(unused)
        {}

        void addBases(QStringList const& list)
        {
            for (QString const& str : list)
            {
                m_bases.insert(str);
            }
        }

        ResourceExtension m_type;
        QSet<QString> m_bases;
        bool m_unused; // need this since its bases can be unused too
    };

    struct Base
    {
        Base(){}
        Base(BaseExtension type, QStringList const& list = QStringList())
            : m_type(type)
        {
            addResources(list);
        }

        void addResources(QStringList const& list)
        {
            for (QString const& str : list)
            {
                m_resources.insert(str);
            }
        }

        BaseExtension m_type;
        QSet<QString> m_resources;
        QSet<QString> m_missingResources;
    };

    typedef QMap<QString, Resource>::iterator ResourceIter;
    QMap<QString, Resource> m_resources;
    bool m_resourceParsed;

    typedef QMap<QString, Base>::iterator BaseIter;
    QMap<QString, Base> m_bases;
    bool m_baseParsed;
};
#endif // GENSARCHIVECLEANER_H
