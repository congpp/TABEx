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
    WelcomeDialog* pDlg = nullptr;
    QString projFile;
    if (argc > 1)
    {
        projFile = QString::fromLocal8Bit(argv[1]);
        w.show();
        w.openProject(projFile);
    }
    else
    {
        pDlg = new WelcomeDialog;
        pDlg->show();
        w.connect(pDlg, &WelcomeDialog::signalOpenProject, &w, &MainWindow::openProject);
    }

    int ret = a.exec();

    for (int i=2; i<argc; ++i)
    {
        QString str = QString::fromLocal8Bit(argv[i]);
        if (str.compare("-RemoveProjOnExit") == 0)
            QFile::remove(projFile);
    }

    if (pDlg) delete pDlg;
    return ret;
}
