#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>


int skok=0;
bool polecenie[3];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ruch()));
    timer->start(80);
    for(int i=0; i<3; i++)
        polecenie[i]=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::ruch()
{
    if(polecenie[0]==true)
    {
        int x = ui->player_1->x()-4;
        ui->player_1->setGeometry(x,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
    }
    if(polecenie[1]==true)
    {
        int x = ui->player_1->x()+4;
        ui->player_1->setGeometry(x,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
    }
    if(polecenie[2]==true)
    {
        int y=0;
        if(skok<5)
        {
            y= ui->player_1->y()-(20-4*skok);
        }
        else
        {
            y= ui->player_1->y()+(4*skok-20);
        }
        if(skok<10) skok++;
        else
        {
            skok=0;
            polecenie[2]=false;
        }
        ui->player_1->setGeometry(ui->player_1->x(),y,ui->player_1->width(),ui->player_1->height());
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left)
    {
        polecenie[0]=true;
        ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_l1.png);");
    }

    if(event->key() == Qt::Key_Right)
    {
        polecenie[1]=true;
        ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_r1.png);");
    }

    if(event->key() == Qt::Key_Up)
    {
        polecenie[2]=true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left)
    {
        polecenie[0]=false;
    }
    if(event->key() == Qt::Key_Right)
    {
        polecenie[1]=false;
    }
}
