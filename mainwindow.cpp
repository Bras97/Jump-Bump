#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu.h"
#include "ui_menu.h"
#include <QPixmap>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <QtNetwork>
#include <string>
#include <QTime>

using namespace std;
int skok=0, skok2=0;

bool polecenie[4];
int poz[4];
int poz2[4];
int PREDKOSC = 5;
int wgore,wdol;
int ILE_BLOKOW=17;
int LIMIT_GRY=500;
int ile_zgonow=0;
QLabel *bloki[17];
int ktory=0, ktory2=0;
int wynik1=0;
bool spada=true;
int respawn[8][2];
string w="";
bool zbity=false;
Menu *menu;
//string host = "127.0.0.1";
QString krolik_l1, krolik_l2, krolik_r1, krolik_r2;


MainWindow::MainWindow(const QString &plec, const QString &imie, const QString &przeciwnik, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ustawienie timera wykonującego funkcję ruchu co 40ms
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ruch()));
    timer->start(40);
    ui->game_over->setVisible(0);

    menu = qobject_cast<Menu *>(parent);
    this->setFixedSize(this->size());

    //wprowadzenie pozycji bloków do tablicy
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

    //domyślnie nie wykonuj żadnego ruchu
    for(int i=0; i<3; i++)
        polecenie[i]=false;
    polecenie[3]=true; //spadaj caly czas jak nie masz gruntu

    //wybor krolika
    if(plec.toStdString()=="M")
    {
        rabbit_l1.load(":/new/prefix1/rabbit2_icon_l1.png");
        rabbit_l2.load(":/new/prefix1/rabbit2_icon_l2.png");
        rabbit_r1.load(":/new/prefix1/rabbit2_icon_r1.png");
        rabbit_r2.load(":/new/prefix1/rabbit2_icon_r2.png");
        rabbit2_l1.load(":/new/prefix1/rabbit1_icon_l1.png");
        rabbit2_l2.load(":/new/prefix1/rabbit1_icon_l2.png");
        rabbit2_r1.load(":/new/prefix1/rabbit1_icon_r1.png");
        rabbit2_r2.load(":/new/prefix1/rabbit1_icon_r2.png");
    }
    else
    {
        rabbit_l1.load(":/new/prefix1/rabbit1_icon_l1.png");
        rabbit_l2.load(":/new/prefix1/rabbit1_icon_l2.png");
        rabbit_r1.load(":/new/prefix1/rabbit1_icon_r1.png");
        rabbit_r2.load(":/new/prefix1/rabbit1_icon_r2.png");
        rabbit2_l1.load(":/new/prefix1/rabbit2_icon_l1.png");
        rabbit2_l2.load(":/new/prefix1/rabbit2_icon_l2.png");
        rabbit2_r1.load(":/new/prefix1/rabbit2_icon_r1.png");
        rabbit2_r2.load(":/new/prefix1/rabbit2_icon_r2.png");
        ui->player_1->setPixmap(rabbit_l1);
        ui->player_2->setPixmap(rabbit2_l1);
        ui->rabbit_1->setStyleSheet("border-image: url(:/new/prefix1/rabbit1.jpg);");
        ui->rabbit_2->setStyleSheet("border-image: url(:/new/prefix1/rabbit2.jpg);");
    }
    //nadanie imion
    ui->name_1->setText(imie);
    ui->name_2->setText(przeciwnik);

    //reset pozycji
    for(int i=0; i<4; i++)
        poz2[i]=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


//sprawdza czy królik nie uderzył w któryś z bloków
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

void delay()
{
    QTime dieTime= QTime::currentTime().addSecs(3);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::koniec_gry(QString napis)
{
    //Przez 3 sekundy wyświetla komunikat końcowy
    ui->game_over->setVisible(1);
    string napis_koncowy = string("WYGRYWA: ");
    ui->game_over->setText( QString::fromStdString(napis_koncowy) + napis);
    delay();

    //czyszczenie i powrót do menu
    delete timer;
    menu->setVisible(true);
    menu->setEnabled(true);
    menu->zeruj();
    this->destroy();
}

void MainWindow::ruch()
{
    //zapobieganie wyrzucenia za mapę
    if((ui->player_1->x()>750) || (ui->player_1->x()<0) || (ui->player_1->y()>500))
            ui->player_1->setGeometry(0,0,ui->player_1->width(),ui->player_1->height());

    //odbiór pozycji z serwera
    if(menu->pozycjeMoje[0] >= 0) {
        poz[0] = menu->pozycjeMoje[0];
        poz[2] = menu->pozycjeMoje[1];
        menu->pozycjeMoje[0] = -1;
    }
    else {
        poz[0]=ui->player_1->x(); //lewo
        poz[2]=ui->player_1->y(); //góra
    }
    //wprowadzenie pozycji pozostałych bloków obrazka z królikiem
    poz[1]=poz[0] + ui->player_1->width(); //prawo
    poz[3]=poz[2] + ui->player_1->height(); //dół

    //ładowanie obrazka ruchomego
    if(menu->pozycjeDrugiego[0]>poz2[0])
    {
        if (ui->player_2->pixmap()->toImage() == rabbit2_r1.toImage())
            ui->player_2->setPixmap(rabbit2_r2);
        else ui->player_2->setPixmap(rabbit2_r1);
    }
    else if(menu->pozycjeDrugiego[0]<poz2[0])
    {
        if (ui->player_2->pixmap()->toImage() == rabbit2_l1.toImage())
            ui->player_2->setPixmap(rabbit2_l2);
        else ui->player_2->setPixmap(rabbit2_l1);
    }

    //pozycja drugiego królika
    poz2[0] = menu->pozycjeDrugiego[0];
    poz2[1]= poz2[0]  + ui->player_2->width(); //prawo
    poz2[2] = menu->pozycjeDrugiego[1];
    poz2[3]= poz2[2] + ui->player_2->height(); //dół

    //qInfo() << poz[0] << " " << poz[2] << " || " << poz2[0] << " " << poz2[2];

    //zmiana pozycji na mapie
    ui->player_1->setGeometry(poz[0],poz[2],ui->player_1->width(),ui->player_1->height());
    ui->player_2->setGeometry(poz2[0],poz2[2],ui->player_2->width(),ui->player_2->height());



    //lewo
    if(polecenie[0]==true)
    {
        //czy nie wychodzi poza mapę
        if(!(poz[0]-PREDKOSC <= 0))
        {
            //jeśli po wykonaniu ruchu nie wpadnie na blok to zmień obrazek królika
            if(!kolizja(poz[0]-PREDKOSC-1, poz[1]-PREDKOSC-1, poz[2], poz[3]))
            {
                if (ui->player_1->pixmap()->toImage() == rabbit_l1.toImage())
                    ui->player_1->setPixmap(rabbit_l2);
                else ui->player_1->setPixmap(rabbit_l1);
            ui->player_1->setGeometry(poz[0]-PREDKOSC,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
            }
            else
                ui->player_1->setGeometry(bloki[ktory]->x()+bloki[ktory]->width()+1,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
                //zrównaj krawędź królika z krawędzią bloku
        }
    }
    //prawo
    if(polecenie[1]==true)
    {
        //czy nie wychodzi poza mapę
        if(!(poz[1]+PREDKOSC >= ui->background->width()))
        {
            //jeśli po wykonaniu ruchu nie wpadnie na blok to zmień obrazek królika
            if(!kolizja(poz[0]+PREDKOSC+1, poz[1]+PREDKOSC+1, poz[2], poz[3]))
            {
                if (ui->player_1->pixmap()->toImage() == rabbit_r1.toImage())
                    ui->player_1->setPixmap(rabbit_r2);
                else ui->player_1->setPixmap(rabbit_r1);
                ui->player_1->setGeometry(poz[0]+PREDKOSC,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
            }
            else
                ui->player_1->setGeometry(bloki[ktory]->x()-ui->player_1->width()-1,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
                //zrównaj krawędź królika z krawędzią bloku
        }
    }
    //skok
    if(polecenie[2]==true)
    {
        wgore=16-skok;
        //sprawdzanie kolizji z blokiem
        if(kolizja(poz[0], poz[1], poz[2]-wgore, poz[3]-wgore))
        {
           skok=16;
           ui->player_1->setGeometry(ui->player_1->x(),(bloki[ktory]->y() + bloki[ktory]->height() + 1), ui->player_1->width(),ui->player_1->height());
        }
        else ui->player_1->setGeometry(ui->player_1->x(),ui->player_1->y()-wgore,ui->player_1->width(),ui->player_1->height());

        //skacz do pewnego pułapu
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
        //jeśli nie dostał sygnału że został zbity to spada
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
    }
    //potrzebne do ominięcia jednej iteracji spadania
    if(zbity==true) zbity=false;

    //przesylanie wyniku
    string pozycje = to_string(poz[0]) + ';' + to_string(poz[2]);
    ui->score_1->setText(QString::fromStdString(to_string(menu->wynik_1)));
    ui->score_2->setText(QString::fromStdString(to_string(menu->wynik_2)));
    menu->write(9,pozycje);

    //czy wygrana
    if(menu->odp_wygrana==0)
    {
        QString napis = ui->name_2->text();
        koniec_gry(napis);
    }
    else if(menu->odp_wygrana==1)
    {
       QString napis = ui->name_1->text();
       koniec_gry(napis);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //jeden ma wyłączyć wszytko(ten co wyszedł) a drugi ma wrócić do menu
    if(menu->odp_zamkniecie==false)
    {
        menu->write(-1,"");
        cout << "Wychodzenie z gry..." << endl;
        qApp->closeAllWindows();
    }
    event->accept();
}

//odblokowywanie odpowiednich ruchów
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




