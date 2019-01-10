#include "menu.h"
#include "ui_menu.h"
#include "mainwindow.h"
#include <iostream>
#include <QMessageBox>
#include <QtGui>
#include <QtNetwork>

using namespace std;
string plec ="M";
string imie = "";
string dlugi_tekst="";
QString przeciwnik="";
QByteArray lista_graczy;
int odp_czy_wolny=-1;
int odp_polaczono=-1;
int odp_zaproszenie=-1;
int odp_lista=-1;
int port = 1234;
int odp_start=-1;

template<class T>
bool isSubstring(const std::basic_string<T> &haystack, const T* needle)
{
    return haystack.find(needle) != std::basic_string<T>::npos;
}



Menu::Menu(QWidget *parent) :
    QDialog(parent),

    tcpSocket(new QTcpSocket(this)),

    ui(new Ui::Menu)
{
    pozycjeMoje[0] = -1;
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &Menu::readData);
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_plecM_clicked()
{
    ui->zdj_krolika->setStyleSheet("border-image: url(:/new/prefix1/rabbit2.jpg);");
}

void Menu::on_plecK_clicked()
{
    ui->zdj_krolika->setStyleSheet("border-image: url(:/new/prefix1/rabbit1.jpg);");
}

void Menu::on_polaczButton_clicked()
{
    QString ip=ui->ipText->text();
    int kropki = count(ip.begin(),ip.end(),'.');
    if(kropki!=3)
    {
        QMessageBox::information(this,"Błąd", "Proszę podać prawidłowy adres IP!");
    }
    else
    {
        tcpSocket->connectToHost(ip, port);

        qApp->processEvents();
        //cout << "Nawiązano połączenie" << nawiazano << endl;

        imie=ui->imieText->text().toStdString();
        if(imie=="")
        {
            QMessageBox::information(this,"Błąd", "Proszę podać imię królika!");
        }
        else if(isSubstring(imie,"#") || isSubstring(imie,";") || isSubstring(imie,"&") || isSubstring(imie,":") )
        {
            QMessageBox::information(this,"Błąd", "Użyto niedozwolonego znaku!");
        }
        else
        {
            if(ui->plecM->isChecked()==false && ui->plecK->isChecked()==false)
                QMessageBox::information(this,"Błąd", "Proszę wybrać płeć królika!");
            else
            {
                if (ui->plecK->isChecked()==true)
                    plec="K";

                write(0,imie);

                do
                {
                    qApp->processEvents();
                }
                while(odp_polaczono==-1);

                if(odp_polaczono==0)
                {
                    QMessageBox::information(this,"Błąd", "Użytkownik z taką nazwą już istnieje");
                }
                else
                {
                    ui->polaczButton->setEnabled(false);
                    ui->imieText->setEnabled(false);
                    ui->plecM->setEnabled(false);
                    ui->plecK->setEnabled(false);
                    ui->listaGraczy->setEnabled(true);
                    ui->listaGraczy->setStyleSheet("background-color: white;");
                    ui->odswiezButton->setEnabled(true);

                }
                odp_polaczono=-1;
            }
        }
    }
}



void Menu::on_listaGraczy_clicked(const QModelIndex &index)
{
    ui->dolaczButton->setEnabled(true);
}


void Menu::on_dolaczButton_clicked()
{
    przeciwnik=ui->listaGraczy->currentItem()->text();
    write(2,przeciwnik.toStdString());
    do
    {
        qApp->processEvents();
    }
    while(odp_czy_wolny==-1);

    if(odp_czy_wolny==0)
        QMessageBox::information(this,"Informacja", "Królik "  + przeciwnik + " skacze teraz z kimś innym. Wybierz innego gracza");
    else if(odp_czy_wolny==1)
    {
        do
        {
            qApp->processEvents();
        }
        while(odp_zaproszenie==-1);
        if(odp_zaproszenie==0)
        {
            QMessageBox::information(this,"Informacja", "Królik "  + przeciwnik + " odrzucił twoje zaproszenie.");
        }
        else if(odp_zaproszenie==1)
        {

            //QMessageBox::information(this,"Informacja", przeciwnik + " przyjął zaproszenie. Kliknij OK by rozpocząć.");
            rozpocznijGre();

        }
    }
}

void Menu::wyslij(string temp)
{
    char * komunikat = new char[temp.size() + 1];
    copy(temp.begin(), temp.end(), komunikat);
    komunikat[temp.size()] = '\0';
    //qInfo() << komunikat << endl;
    tcpSocket->write(komunikat);
    delete[] komunikat;

}

void Menu::write(int polecenie, string nick)
{
    odp_start=1;
    if(tcpSocket->state() == QAbstractSocket::ConnectedState) {
        switch(polecenie)
        {
            case 0: wyslij(string("#0;" + nick + "&"));
                    break;
            case 1: wyslij(string("#1;1&"));
                    break;
            case 2: wyslij(string("#2;" + nick + "&"));
                    break;
            case 3: wyslij(string("#3;" + nick + "&"));
                    break;
            case 4: wyslij(string("#4;" + nick + "&"));
                    break;
            case 9: wyslij(string("#9;" + nick + "&"));
                    break;
            default: break;
        }

    }
}

QByteArray Menu::scalanie()
{
    string komunikat="";
    int usun=0;
    bool zapisuj=false, poprawny=false;
    for(int i=0; i<dlugi_tekst.size(); i++)
    {
        if(dlugi_tekst[i]=='&' && zapisuj == true)
        {
            poprawny=true;
            usun=i+1;
            break;
        }
        if (zapisuj==true)
            komunikat+=dlugi_tekst[i];
        if (dlugi_tekst[i]=='#')
        {
            zapisuj=true;
        }
    }
    if(poprawny==true)
    {
        dlugi_tekst.erase(0,usun);
        //cout << "Komunikat: " << komunikat << endl;
        poprawny=false;
    }
    else
        komunikat="";
    QByteArray byteArray(komunikat.c_str(), komunikat.length());
    return byteArray;

}

void Menu::readData()
{
     QByteArray komunikat;
     do
     {
         komunikat ="";
         QByteArray temp = tcpSocket->read(10);
         dlugi_tekst+=temp.toStdString();

         komunikat = scalanie();
         if(komunikat!="")
         {
             QList<QByteArray> qlist = komunikat.split(';');

             int numer = qlist[0].toInt();
             QByteArray tresc = qlist[1];
             switch(numer)
             {
                 case -1:
                 {
                     QMessageBox::information(this,"Informacja", "Przeciwnik rozłączył się");

                     game->close();


                     this->setVisible(true);
                     this->setEnabled(true);
                     break;
                 }
                 case 0:
                {
                     (tresc=="0") ? odp_polaczono=0 : odp_polaczono=1;
                     break;
                }
                case 1:
                {
                     lista_graczy = tresc;
                     odp_lista=0;
                     break;
                }
                case 2:
                {
                    (tresc=="0") ? odp_czy_wolny=0 : odp_czy_wolny=1;
                    break;
                }
                case 3:
                {
                     QMessageBox::StandardButton reply;
                     reply = QMessageBox::question(this, "Zaproszenie", "Królik " + tresc + " zaprasza do gry. Akceptujesz?",
                                                   QMessageBox::Yes|QMessageBox::No);
                     if (reply == QMessageBox::Yes) {
                         przeciwnik=tresc;
                         write(3,string(tresc+";1"));
                         cout << "Challange accepted" <<endl;
                         rozpocznijGre();
                     } else {
                         write(3,string(tresc+";0"));
                         cout << ":(" << endl;
                     }
                     break;
                }
                case 4:
                {
                    //qInfo() << "4";
                    if(tresc=="0") {
                        write(4,"0");
                        odp_zaproszenie=0;
                    }
                    else {
                        write(4,"1");
                        odp_zaproszenie=1;
                        //rozpocznijGre();
                    }
                    break;
                }
             case 8:
             {
    //                qInfo() << "9 " << qlist[1] << " " << qlist[2];
                pozycjeMoje[0] = qlist[1].toInt();
                pozycjeMoje[1] = qlist[2].toInt();
                break;
             }

             case 9:
             {
//                qInfo() << "9 " << qlist[1] << " " << qlist[2];
                pozycjeDrugiego[0] = qlist[1].toInt();
                pozycjeDrugiego[1] = qlist[2].toInt();
                break;
             }
             default: break;
             }
         }
    }
    while(komunikat!="");

}





void Menu::on_odswiezButton_clicked()
{
    write(1,imie);
    do
    {
        qApp->processEvents();
    }
    while(odp_lista==-1);

    ui->listaGraczy->clear();
    qInfo() << lista_graczy << "///" << lista_graczy.size();
    if(lista_graczy.size() > 0)
    {
        QList<QByteArray> lista_graczy_podzielona=lista_graczy.split(':');
        for(int i=0; i<lista_graczy_podzielona.size()-1; i++)
            ui->listaGraczy->addItem(lista_graczy_podzielona[i]);
    }
    else
        QMessageBox::information(this,"Informacja", "Nie ma jeszcze żadnego gracza. Poczekaj chwilę");

    lista_graczy.clear();
    odp_lista = -1;
}

void Menu::rozpocznijGre()
{
    this->setVisible(false);
    this->setEnabled(false);
    this->close();

    game = new MainWindow(QString::fromStdString(plec), QString::fromStdString(imie), przeciwnik, this);
    game->show();
}

void Menu::on_ipButton_clicked()
{
    tcpSocket->connectToHost("127.0.0.1", port);
    cout << "Nawiązano połączenie" << endl;

}
