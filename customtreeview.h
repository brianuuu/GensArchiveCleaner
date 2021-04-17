#ifndef CUSTOMTREEVIEW_H
#define CUSTOMTREEVIEW_H

#include <QTreeView>

class CustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    using QTreeView::QTreeView;
    void resizeEvent(QResizeEvent* e) override;

    void updateColumnWidth();
};

#endif // CUSTOMTREEVIEW_H
