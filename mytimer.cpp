#include "mytimer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QApplication>
#include <QDebug>

Ui::MainWindow *ui;

MyTimer::MyTimer(Ui::MainWindow *u)
{
    ui=u;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(MySlot()));

    timer-> start(1000);
}

void MyTimer::MySlot()
{
    qDebug() << "Timer executed";

    int x= ui->player_1->x()+3;
    ui->player_1->setGeometry(x,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
}
