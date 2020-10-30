#include <QCoreApplication>
#include "itest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    RUN_TEST(argc, argv);
    return a.exec();
}
