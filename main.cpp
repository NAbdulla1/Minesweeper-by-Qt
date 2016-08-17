#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedWidth(250);
    w.setFixedHeight(300);
    w.show();

    return a.exec();
}
