#include "mainwindow.h"
#include "menu.h"
#include <iostream>
#include <QApplication>


using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Menu m;
    m.show();
    MainWindow w;// = new MainWindow();
    w.show();
    return a.exec();

}
