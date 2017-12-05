#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <cubature.h>

#include "../math_module/math_module.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.start();

    return a.exec();
}
