#include "gensarchivecleaner.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GensArchiveCleaner w;
    w.show();
    return a.exec();
}
