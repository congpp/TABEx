#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString projFile;
    if (argc > 1)
    {
        projFile = QString::fromLocal8Bit(argv[1]);
        w.openProject(projFile);
    }

    int ret = a.exec();

    for (int i=2; i<argc; ++i)
    {
        QString str = QString::fromLocal8Bit(argv[i]);
        if (str.compare("-RemoveProjOnExit") == 0)
            QFile::remove(projFile);
    }

    return ret;
}
