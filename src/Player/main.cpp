#include "mainwindow.h"
#include "resourceloader.h"
#include "welcomedialog.h"
#include <QApplication>
#include <QFile>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //资源一次性load成QImage
    g_resLoader.loadImages();

    MainWindow w;
    w.show();
    QString projFile;
    //if (argc > 1)
    //{
    //    projFile = QString::fromLocal8Bit(argv[1]);
    //    w.show();
    //    w.openProject(projFile);
    //}
    //else
    //{
    //    WelcomeDialog dlg;
    //    dlg.exec();
    //    projFile = dlg.getSelectedProjFile();
    //    if (!projFile.isEmpty())
    //    {
    //        w.show();
    //        w.openProject(projFile);
    //    }
    //    else
    //    {
    //        exit(0);
    //    }
    //}

    int ret = a.exec();

    for (int i=2; i<argc; ++i)
    {
        QString str = QString::fromLocal8Bit(argv[i]);
        if (str.compare("-RemoveProjOnExit") == 0)
            QFile::remove(projFile);
    }

    return ret;
}
