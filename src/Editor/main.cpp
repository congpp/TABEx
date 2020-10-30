#include "mainwindow.h"
#include "../Project/tabproject.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc > 1)
    {
        QString filePath = QString::fromLocal8Bit(argv[1]);
        TAB_INST->openProject(filePath);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
