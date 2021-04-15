#include "customfilesystemmodel.h"

QVariant CustomFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole)
    {
        // unused has higher priority
        if (m_unusedList.contains(index))
        {
            return m_unusedBrush;
        }
        else if (m_errorList.contains(index))
        {
            return m_errorBrush;
        }
        else
        {
            return QBrush(Qt::black);
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        if (m_errorList.contains(index))
        {
            QString str = "<font color='red'><b>Missing Resources:</b></font>";
            str += "<ul>";
            for (QString const& resource : m_errorList[index])
            {
                str += "<li>" + resource + "</li>";
            }
            str += "</ul>";
            str += "<p style=\"color:rgba(0,0,0,0);font-size:1px\">" + QString::number(index.row()) + "</p>";
            return str;
        }
        else if (m_unusedList.contains(index))
        {
            return "<p style=\"color:rgb(255,99,71);\"><b>Potentially Unused</b></p>";
        }
        else
        {
            return "";
        }
    }

    return QFileSystemModel::data(index, role);
}

bool CustomFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::ForegroundRole)
    {
        if (value == m_errorBrush)
        {
            // Nothing
        }
        else
        {
            m_errorList.remove(index);
        }

        if (value == m_unusedBrush)
        {
            m_unusedList.insert(index);
        }
        else
        {
            m_unusedList.remove(index);
        }

        emit dataChanged(index, index, {Qt::ForegroundRole});
        return true;
    }

    return QFileSystemModel::setData(index, value, role);
}

bool CustomFileSystemModel::setError(const QModelIndex &index, const QSet<QString>& list)
{
    m_errorList.insert(index, list);
    return setData(index, m_errorBrush, Qt::ForegroundRole);
}

bool CustomFileSystemModel::setUnused(const QModelIndex &index)
{
    return setData(index, m_unusedBrush, Qt::ForegroundRole);
}

bool CustomFileSystemModel::setNormal(const QModelIndex &index)
{
    return setData(index, QBrush(Qt::black), Qt::ForegroundRole);
}

void CustomFileSystemModel::clearList()
{
    m_errorList.clear();
    m_unusedList.clear();
}
