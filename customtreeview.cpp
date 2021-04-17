#include "customtreeview.h"

void CustomTreeView::resizeEvent(QResizeEvent *e)
{
    updateColumnWidth();
    QTreeView::resizeEvent(e);
}

void CustomTreeView::updateColumnWidth()
{
    this->setColumnWidth(0, this->width() - this->columnWidth(2) - 30);
}
