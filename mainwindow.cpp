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
bool polecenie[4];
int pola_zabronione[1][4];
int poz[4];
int PREDKOSC = 5;
int wgore,wdol;
int ILE_BLOKOW=3;
QLabel *bloki[3];
int ktory=0;
bool spada=true;

void MainWindow::detekcja(QLabel *blok)
{
    pola_zabronione[0][0] = blok->x();                  //lewa
    pola_zabronione[0][1] = blok->x() + blok->width();  //prawa
    pola_zabronione[0][2] = blok->y();                  //góra
    pola_zabronione[0][3] = blok->y() + blok->height(); //dół

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ruch()));
    timer->start(40);
    detekcja(ui->block_1);

    bloki[0] = ui->block_1;
    bloki[1] = ui->block_2;
    bloki[2] = ui->block_3;
    for(int i=0; i<3; i++)
        polecenie[i]=false;
    polecenie[3]=true; //spadaj caly czas jak nie masz gruntu
    for(int i=0; i<4; i++)
        cout << pola_zabronione[0][i] << endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::kolizja(int poz0, int poz1,int poz2,int poz3)
{
    for(int i=0; i<ILE_BLOKOW; i++)
    {
        if( ( poz0 <= (bloki[i]->x()+bloki[i]->width()) ) && ( poz1 >= bloki[i]->x() ) &&
            ( poz2 <= (bloki[i]->y()+bloki[i]->height()) ) && ( poz3 >= bloki[i]->y() ) )
        {
            ktory=i;
            return true;
        }
    }
    return false;


}

void MainWindow::ruch()
{
    poz[0]=ui->player_1->x(); //lewo
    poz[1]=ui->player_1->x() + ui->player_1->width(); //prawo
    poz[2]=ui->player_1->y(); //góra
    poz[3]=ui->player_1->y() + ui->player_1->height(); //dół

    //lewo
    if(polecenie[0]==true)
    {
        if(!(poz[0]-PREDKOSC <= 0))
        {
            if(!kolizja(poz[0]-PREDKOSC, poz[1]-PREDKOSC, poz[2], poz[3]))
            {
                if (ui->player_1->styleSheet() == "border-image: url(:/new/prefix1/rabbit2_icon_l1.png);")
                    ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_l2.png);");
                else ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_l1.png);");
            ui->player_1->setGeometry(poz[0]-PREDKOSC,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
            }
            else
                ui->player_1->setGeometry(bloki[ktory]->x()+bloki[ktory]->width()+1,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
        }
    }
    //prawo
    if(polecenie[1]==true)
    {
        if(!(poz[1]+PREDKOSC >= ui->background->width()))
        {
            if(!kolizja(poz[0]+PREDKOSC, poz[1]+PREDKOSC, poz[2], poz[3]))
            {
                if (ui->player_1->styleSheet() == "border-image: url(:/new/prefix1/rabbit2_icon_r1.png);")
                    ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_r2.png);");
                else ui->player_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit2_icon_r1.png);");
                ui->player_1->setGeometry(poz[0]+PREDKOSC,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
            }
            else
                ui->player_1->setGeometry(bloki[ktory]->x()-ui->player_1->width()-1,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
        }
    }
    //skok
    if(polecenie[2]==true)
    {
        wgore=16-skok;

        if(kolizja(poz[0], poz[1], poz[2]-wgore, poz[3]-wgore))
        {
           skok=16;
           ui->player_1->setGeometry(ui->player_1->x(),(bloki[ktory]->y() + bloki[ktory]->height() + 1), ui->player_1->width(),ui->player_1->height());
        }
        else ui->player_1->setGeometry(ui->player_1->x(),ui->player_1->y()-wgore,ui->player_1->width(),ui->player_1->height());

        if(skok<16) skok++;
        else
        {
            skok=0;
            polecenie[2]=false;
            polecenie[3]=true;
        }
    }
    //spadanie
    if(polecenie[3]==true)
    {
        if(kolizja(poz[0], poz[1], poz[2]+5, poz[3]+5))
        {
           ui->player_1->setGeometry(ui->player_1->x(),(bloki[ktory]->y() - ui->player_1->height() -1), ui->player_1->width(),ui->player_1->height());
           spada=false;
        }
        else
        {
            ui->player_1->setGeometry(ui->player_1->x(),ui->player_1->y()+5,ui->player_1->width(),ui->player_1->height());
            spada=true;
        }
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

    if(event->key() == Qt::Key_Up && polecenie[2] == false && spada == false)
    {
        polecenie[2]=true;
        polecenie[3]=false;
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
