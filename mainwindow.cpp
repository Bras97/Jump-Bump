#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <string>

using namespace std;
int skok=0;
bool polecenie[3];
int pola_zabronione[1][4];
int pozycja[4];

void MainWindow::detekcja(QLabel *blok)
{
    pola_zabronione[0][0] = blok->x();
    pola_zabronione[0][1] = blok->y();
    pola_zabronione[0][2] = blok->x() + blok->width();
    pola_zabronione[0][3] = blok->y() + blok->height();

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ruch()));
    timer->start(80);
    detekcja(ui->block_1);
    for(int i=0; i<3; i++)
        polecenie[i]=false;
    for(int i=0; i<4; i++)
        cout << pola_zabronione[0][i] << endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::ruch()
{
    pozycja[0]=ui->player_1->x();
    pozycja[1]=ui->player_1->y();
    pozycja[2]=ui->player_1->x() + ui->player_1->width();
    pozycja[3]=ui->player_1->y() + ui->player_1->height();

    //lewo
    if(polecenie[0]==true)
    {
        if (ui->player_1->styleSheet() == "border-image: url(:/new/prefix1/rabbit2_icon_l1.png);")
            ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_l2.png);");
        else ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_l1.png);");
        int x = ui->player_1->x()-10;
        ui->player_1->setGeometry(x,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
    }
    //prawo
    if(polecenie[1]==true)
    {
        bool wykonaj=true;
        for(int i=0; i<1; i++)
        {
            if((pozycja[2]>pola_zabronione[i][0]) &&
               (pozycja[3]>pola_zabronione[i][1]) &&
               (pozycja[1]<pola_zabronione[i][3]))
                wykonaj = false;
        }
        if(wykonaj==true)
        {
            if (ui->player_1->styleSheet() == "border-image: url(:/new/prefix1/rabbit2_icon_r1.png);")
                ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_r2.png);");
            else ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_r1.png);");
            int x = ui->player_1->x()+10;
            ui->player_1->setGeometry(x,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
        }
    }
    //skok
    if(polecenie[2]==true)
    {
        int y=0;
        if(skok<8)
        {
            y= ui->player_1->y()-(32-4*skok);
        }
        else
        {
            y= ui->player_1->y()+(4*skok-32);
        }
        if(skok<16) skok++;
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
    }

    if(event->key() == Qt::Key_Right)
    {
        polecenie[1]=true;
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
