#ifndef CUSTOMFILESYSTEMMODEL_H
#define CUSTOMFILESYSTEMMODEL_H

#include <QBrush>
#include <QFileSystemModel>
#include <QSet>

class CustomFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    using QFileSystemModel::QFileSystemModel;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &varData, int role) override;

    bool setError(const QModelIndex &index, const QSet<QString>& list);
    bool setUnused(const QModelIndex &index);
    bool setNormal(const QModelIndex &index);
    void clearList();

private:
    QMap<QPersistentModelIndex,QSet<QString>> m_errorList;
    QSet<QPersistentModelIndex> m_unusedList;

    QBrush const m_errorBrush = QBrush(Qt::red);
    QBrush const m_unusedBrush = QBrush(QColor(255,99,71));
};

#endif // CUSTOMFILESYSTEMMODEL_H
