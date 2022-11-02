#include "labcd.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LabCD w;
    w.show();
    return a.exec();
}
