#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <string>

using namespace std;
int skok=0, skok2=0;

bool polecenie[8];
int poz[4];
int poz2[4];
int PREDKOSC = 5;
int wgore,wdol,wgore2,wdol2;
int ILE_BLOKOW=17;
int LIMIT_GRY=10;
QLabel *bloki[17];
int ktory=0, ktory2=0;
int wynik1=0,wynik2=0;
bool spada=true, spada2=true;
int respawn[8][2];
string w="",w2="";
bool zbity2=false, zbity=false;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ruch()));
    timer->start(40);
    ui->game_over->setVisible(0);

    bloki[0] = ui->block_1;
    bloki[1] = ui->block_2;
    bloki[2] = ui->block_3;
    bloki[3] = ui->block_4;
    bloki[4] = ui->block_5;
    bloki[5] = ui->block_6;
    bloki[6] = ui->block_7;
    bloki[7] = ui->block_8;
    bloki[8] = ui->block_9;
    bloki[9] = ui->block_10;
    bloki[10] = ui->block_11;
    bloki[11] = ui->block_12;
    bloki[12] = ui->block_13;
    bloki[13] = ui->block_14;
    bloki[14] = ui->block_15;
    bloki[15] = ui->block_16;
    bloki[16] = ui->block_17;
    respawn[0][0]=0; respawn[0][1]=20;
    respawn[1][0]=530; respawn[1][1]=390;
    respawn[2][0]=650; respawn[2][1]=410;
    respawn[3][0]=690; respawn[3][1]=30;
    respawn[4][0]=250; respawn[4][1]=10;
    respawn[5][0]=310; respawn[5][1]=220;
    respawn[6][0]=100; respawn[6][1]=420;
    respawn[7][0]=0; respawn[7][1]=250;

    for(int i=0; i<8; i++)
        polecenie[i]=false;
    polecenie[3]=true; //spadaj caly czas jak nie masz gruntu
    polecenie[7]=true; //spadaj caly czas jak nie masz gruntu
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
            ktory2=ktory=i;
            return true;
        }
    }
    return false;
}

bool MainWindow::zderzenie(int poz0, int poz1,int poz2,int poz3, int pozd0, int pozd1,int pozd2,int pozd3)
{
    for(int i=0; i<ILE_BLOKOW; i++)
    {
        if( ( poz0 <= pozd1 ) && ( poz1 >= pozd0 ) &&
            ( poz2 <= pozd3 ) && ( poz3 >= pozd2 ) )
        {
            if(poz3<=pozd3-15 && poz1>=pozd0+10 && poz0<=pozd1-10)
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

    poz2[0]=ui->player_2->x(); //lewo
    poz2[1]=ui->player_2->x() + ui->player_2->width(); //prawo
    poz2[2]=ui->player_2->y(); //góra
    poz2[3]=ui->player_2->y() + ui->player_2->height(); //dół

    //lewo
    if(polecenie[0]==true)
    {
        if(!(poz[0]-PREDKOSC <= 0))
        {
            if(!kolizja(poz[0]-PREDKOSC-1, poz[1]-PREDKOSC-1, poz[2], poz[3]))
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
            if(!kolizja(poz[0]+PREDKOSC+1, poz[1]+PREDKOSC+1, poz[2], poz[3]))
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
        if(zderzenie(poz[0], poz[1], poz[2], poz[3], poz2[0], poz2[1], poz2[2], poz2[3]))
        {
            wynik1++;
            if (wynik1<10)
                w = "0" + to_string(wynik1);
            else
                w = to_string(wynik1);
            ui->score_1->setText(QString::fromStdString(w));
            if (wynik1==LIMIT_GRY)
            {

                ui->game_over->setVisible(1);
                QString napis = ui->name_1->text();
                string napis_koncowy = string("WYGRYWA: ");
                ui->game_over->setText( QString::fromStdString(napis_koncowy) + napis);
                //ui->game_over->setAlignment(1);
            }
            zbity2=true;
            srand(time(NULL));
            int spawn= rand() % 8;
            ui->player_2->setGeometry(respawn[spawn][0],respawn[spawn][1],ui->player_2->width(),ui->player_2->height());
        }
        if(zbity==false)
        {
            skok++;
            if(kolizja(poz[0], poz[1], poz[2]+skok, poz[3]+skok))
            {
               ui->player_1->setGeometry(ui->player_1->x(),(bloki[ktory]->y() - ui->player_1->height() -1), ui->player_1->width(),ui->player_1->height());
               spada=false;
               skok=0;
            }
            else
            {
                ui->player_1->setGeometry(ui->player_1->x(),ui->player_1->y()+skok,ui->player_1->width(),ui->player_1->height());
                spada=true;
            }
        }
        else zbity=false;
    }



    //lewo
    if(polecenie[4]==true)
    {
        if(!(poz2[0]-PREDKOSC <= 0))
        {
            if(!kolizja(poz2[0]-PREDKOSC-1, poz2[1]-PREDKOSC-1, poz2[2], poz2[3]))
            {
                if (ui->player_2->styleSheet() == "border-image: url(:/new/prefix1/rabbit1_icon_l1.png);")
                    ui->player_2->setStyleSheet("border-image: url(:/new/prefix1/rabbit1_icon_l2.png);");
                else ui->player_2->setStyleSheet("border-image: url(:/new/prefix1/rabbit1_icon_l1.png);");
            ui->player_2->setGeometry(poz2[0]-PREDKOSC,ui->player_2->y(),ui->player_2->width(),ui->player_2->height());
            }
            else
                ui->player_2->setGeometry(bloki[ktory2]->x()+bloki[ktory2]->width()+1,ui->player_2->y(),ui->player_2->width(),ui->player_2->height());
        }
    }
    //prawo
    if(polecenie[5]==true)
    {
        if(!(poz2[1]+PREDKOSC >= ui->background->width()))
        {
            if(!kolizja(poz2[0]+PREDKOSC+1, poz2[1]+PREDKOSC+1, poz2[2], poz2[3]))
            {
                if (ui->player_2->styleSheet() == "border-image: url(:/new/prefix1/rabbit1_icon_r1.png);")
                    ui->player_2->setStyleSheet("border-image: url(:/new/prefix1/rabbit1_icon_r2.png);");
                else ui->player_2->setStyleSheet("border-image: url(:/new/prefix1/rabbit1_icon_r1.png);");
                ui->player_2->setGeometry(poz2[0]+PREDKOSC,ui->player_2->y(),ui->player_2->width(),ui->player_2->height());
            }
            else
                ui->player_2->setGeometry(bloki[ktory2]->x()-ui->player_2->width()-1,ui->player_2->y(),ui->player_2->width(),ui->player_2->height());
        }
    }
    //skok2
    if(polecenie[6]==true)
    {
        wgore2=16-skok2;

        if(kolizja(poz2[0], poz2[1], poz2[2]-wgore2, poz2[3]-wgore2))
        {
           skok2=16;
           ui->player_2->setGeometry(ui->player_2->x(),(bloki[ktory2]->y() + bloki[ktory2]->height() + 1), ui->player_2->width(),ui->player_2->height());
        }
        else ui->player_2->setGeometry(ui->player_2->x(),ui->player_2->y()-wgore2,ui->player_2->width(),ui->player_2->height());

        if(skok2<16) skok2++;
        else
        {
            skok2=0;
            polecenie[6]=false;
            polecenie[7]=true;
        }
    }
    //spadanie
    if(polecenie[7]==true)
    {
        if(zderzenie(poz2[0], poz2[1], poz2[2], poz2[3], poz[0], poz[1], poz[2], poz[3]))
        {
            wynik2++;
            if (wynik2<10)
                w2 = "0" + to_string(wynik2);
            else
                w2 = to_string(wynik2);
            ui->score_2->setText(QString::fromStdString(w2));
            if (wynik2==LIMIT_GRY)
            {

                ui->game_over->setVisible(1);
                QString napis = ui->name_2->text();
                string napis_koncowy = string("WYGRYWA: ");
                ui->game_over->setText( QString::fromStdString(napis_koncowy) + napis);
                //ui->game_over->setAlignment(1);
            }
            zbity=true;
            srand(time(NULL));
            int spawn= rand() % 8;
            ui->player_1->setGeometry(respawn[spawn][0],respawn[spawn][1],ui->player_1->width(),ui->player_1->height());
        }
        if(zbity2==false)
        {
            skok2++;
            if(kolizja(poz2[0], poz2[1], poz2[2]+skok2, poz2[3]+skok2))
            {
               ui->player_2->setGeometry(ui->player_2->x(),(bloki[ktory2]->y() - ui->player_2->height() -1), ui->player_2->width(),ui->player_2->height());
               spada2=false;
               skok2=0;
            }
            else
            {
                ui->player_2->setGeometry(ui->player_2->x(),ui->player_2->y()+skok2,ui->player_2->width(),ui->player_2->height());
                spada2=true;
            }
        }
        else zbity2=false;
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

    if(event->key() == Qt::Key_A)
    {
        polecenie[4]=true;
    }

    if(event->key() == Qt::Key_D)
    {
        polecenie[5]=true;
    }

    if(event->key() == Qt::Key_W && polecenie[6] == false && spada2 == false)
    {
        polecenie[6]=true;
        polecenie[7]=false;
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
    if(event->key() == Qt::Key_A)
    {
        polecenie[4]=false;
    }
    if(event->key() == Qt::Key_D)
    {
        polecenie[5]=false;
    }
}
