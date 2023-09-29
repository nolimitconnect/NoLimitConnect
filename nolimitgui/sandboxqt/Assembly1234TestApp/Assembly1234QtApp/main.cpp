#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qWarning() << "main was entered";
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
