#include "customfilesystemmodel.h"

QVariant CustomFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole)
    {
        QString const fileName = this->fileName(index);
        if (m_errorList.contains(fileName))
        {
            return m_errorBrush;
        }
        else if (m_unusedList.contains(fileName))
        {
            return m_unusedBrush;
        }
        else
        {
            return m_usedBrush;
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        QString const fileName = this->fileName(index);
        QString str = "";
        if (m_errorList.contains(fileName))
        {
            str = "<font color='red'><b>Missing resources:</b></font>";
            str += "<ul>";
            for (QString const& resource : m_errorList[fileName])
            {
                str += "<li>" + resource + "</li>";
            }
            str += "</ul>";
        }

        if (m_usedList.contains(fileName))
        {
            str = "<b>Used by:</b>";
            str += "<ul>";
            for (QString const& base : m_usedList[fileName])
            {
                str += "<li>" + base + "</li>";
            }
            str += "</ul>";
        }

        if (m_unusedList.contains(fileName))
        {
            str += "<p style=\"color:rgb(30,144,255);\"><b>Potentially unused</b></p>";
        }

        if (!str.isEmpty())
        {
            str += "<p style=\"color:rgba(0,0,0,0);font-size:1px\">" + QString::number(index.row()) + "</p>";
        }

        return str;
    }

    return QFileSystemModel::data(index, role);
}

bool CustomFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::ForegroundRole)
    {
        if (value != m_errorBrush && value != m_usedBrush && value != m_unusedBrush)
        {
            QString const fileName = this->fileName(index);
            m_errorList.remove(fileName);
            m_usedList.remove(fileName);
            m_unusedList.remove(fileName);
        }

        emit dataChanged(index, index, {Qt::ForegroundRole});
        return true;
    }

    return QFileSystemModel::setData(index, value, role);
}

bool CustomFileSystemModel::setError(const QModelIndex &index, const QSet<QString>& list)
{
    m_errorList.insert(this->fileName(index), list);
    return setData(index, m_errorBrush, Qt::ForegroundRole);
}

bool CustomFileSystemModel::setUsed(const QModelIndex &index, const QSet<QString> &list)
{
    m_usedList.insert(this->fileName(index), list);
    return setData(index, m_usedBrush, Qt::ForegroundRole);
}

bool CustomFileSystemModel::setUnused(const QModelIndex &index)
{
    m_unusedList.insert(this->fileName(index));
    return setData(index, m_unusedBrush, Qt::ForegroundRole);
}

bool CustomFileSystemModel::setNormal(const QModelIndex &index)
{
    return setData(index, m_usedBrush, Qt::ForegroundRole);
}

void CustomFileSystemModel::clearList()
{
    m_errorList.clear();
    m_usedList.clear();
    m_unusedList.clear();
}
