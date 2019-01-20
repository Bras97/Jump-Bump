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
int odp_start=-1;
int port = 1234;

//funkcja do szukania podstringów w stringu
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

    //aktywacja slotu zczytywania pobranych danych
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &Menu::readData);

    //ładowanie logo
    QMovie *movie = new QMovie(":/new/prefix1/logo.gif");
    ui->logo->setMovie(movie);
    movie->start();
    this->setFixedSize(this->size());
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

    //weryfikacja IP
    if(kropki!=3)
    {
        QMessageBox::information(this,"Błąd", "Proszę podać prawidłowy adres IP!");
    }
    else
    {
        //weryfikacja czy podano po IP port
        if(isSubstring(ip.toStdString(),":"))
        {

            QList<QString> qlist = ip.split(":");
            port=qlist[1].toInt();
            ip=qlist[0];
        }
        tcpSocket->connectToHost(ip, port);

        //poczekaj na połączenie
        qApp->processEvents();

        //weryfikacja poprawności imienia
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

            //weryfikacja płci króliczej
            if(ui->plecM->isChecked()==false && ui->plecK->isChecked()==false)
                QMessageBox::information(this,"Błąd", "Proszę wybrać płeć królika!");
            else
            {
                if (ui->plecK->isChecked()==true)
                    plec="K";

                //sprawdzanie czy nazwa jest dozwolona
                write(0,imie);

                //czekaj na odpowiedź
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
                    //jeśli nazwa jest dostępna to zablokuj wcześniejsze okna i odblokuj listę graczy
                    ui->polaczButton->setEnabled(false);
                    ui->imieText->setEnabled(false);
                    ui->plecM->setEnabled(false);
                    ui->plecK->setEnabled(false);
                    ui->ipText->setEnabled(false);
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

    //sprawdź czy wybrany królik jest wolny
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
        //jeśli królik wolny to wyślij zaproszenie
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
            rozpocznijGre();
        }
    }
    odp_czy_wolny=-1;
    odp_zaproszenie=-1;
    odp_start=-1;
}

void Menu::wyslij(string temp)
{
    //wysłanie wiadomości na serwer jako ciąg charów
    char * komunikat = new char[temp.size() + 1];
    copy(temp.begin(), temp.end(), komunikat);
    komunikat[temp.size()] = '\0';
    tcpSocket->write(komunikat);
    delete[] komunikat;

}

void Menu::write(int polecenie, string nick)
{
    odp_start=1;
    //jeśli serwer jest dostępny to wyślij któryś z otrzymanych komunikatów
    if(tcpSocket->state() == QAbstractSocket::ConnectedState) {
        switch(polecenie)
        {
            case -1: wyslij(string("#-1;" + nick + "&"));
                    break;
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

    //funkcja znajduje znak początkowy # i przepisuje pojedyncze znaki do momentu aż do znalezienia znaku końcowego &,
    //jeśli nie znajdzie to funkcja zwraca pusty komunikat, w przeciwnym razie zwraca komunikat oraz wycina z dlugiego_tekstu
    //(czyli wszystkiego co zostało odebrane) wszystkie znaki aż do znalezionego &
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
        //        cout << "Komunikat: " << komunikat << endl;
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

     //dopóki komunikat nie jest pusty wykonuj operacje
     do
     {
         komunikat ="";

         //odbierz 10 znaków
         QByteArray temp = tcpSocket->read(10);

         //dodaj do jednego długiego ciągu
         dlugi_tekst+=temp.toStdString();
//         cout << dlugi_tekst << '\n';

         //wyszukaj komunikat
         komunikat = scalanie();

         if(komunikat!="")
         {
             QList<QByteArray> qlist = komunikat.split(';');

             //podziel wiadomość na numer komunikatu i treść
             int numer = qlist[0].toInt();
             QByteArray tresc = qlist[1];
             //w zależności od otrzymanego numeru wykonaj odpowiednie instrukcje
             switch(numer)
             {
                 //zamknij planszę gdy przeciwnik się rozłączy
                 case -1:
                 {
                     QMessageBox::information(this,"Informacja", "Przeciwnik rozłączył się");
                     odp_zamkniecie = true;
                     game->close();


                     this->setVisible(true);
                     this->setEnabled(true);
                     break;
                 }
                 //sprawdź zajętość nicku
                 case 0:
                {
                     (tresc=="0") ? odp_polaczono=0 : odp_polaczono=1;
                     break;
                }
                 //wyświetl listę graczy
                case 1:
                {
                     lista_graczy = tresc;
                     odp_lista=0;
                     break;
                }
                 //sprawdź czy królik jest wolny
                case 2:
                {
                    (tresc=="0") ? odp_czy_wolny=0 : odp_czy_wolny=1;
                    break;
                }
                 //zareaguj na otrzymane zaproszenie
                case 3:
                {
                     QMessageBox::StandardButton reply;
                     reply = QMessageBox::question(this, "Zaproszenie", "Królik " + tresc + " zaprasza do gry. Akceptujesz?",
                                                   QMessageBox::Yes|QMessageBox::No);
                     if (reply == QMessageBox::Yes) {
                         przeciwnik=tresc;
                         //wyslij zgodę na grę
                         write(3,string(tresc+";1"));
                         cout << "Challange accepted" <<endl;
                         rozpocznijGre();
                     } else {
                         //nie zgódź się na wspólną grę
                         write(3,string(tresc+";0"));
                         cout << ":(" << endl;
                     }
                     break;
                }
                 //zobacz czy przeciwnik się zgodził
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
                 //zapisz do zmiennej inf o zakończonej grze
                 case 6:
                 {
                     odp_wygrana=qlist[1].toInt();
                     break;
                 }
                 //odbierz wynik
                 case 7:
                 {
        //                qInfo() << "9 " << qlist[1] << " " << qlist[2];
                    wynik_1 = qlist[1].toInt();
                    wynik_2 = qlist[2].toInt();
                    break;
                 }
                 //odbierz moją pozycję
                 case 8:
                 {
        //                qInfo() << "9 " << qlist[1] << " " << qlist[2];
                    pozycjeMoje[0] = qlist[1].toInt();
                    pozycjeMoje[1] = qlist[2].toInt();
                    break;
                 }
                 //odbierz pozycję przeciwnika
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
    //wyślij prośbę o listę graczy
    write(1,imie);
    do
    {
        qApp->processEvents();
    }
    while(odp_lista==-1);

    ui->listaGraczy->clear();
    qInfo() << lista_graczy << "///" << lista_graczy.size();

    //wyświetl listę jeśli jest ktoś jeszcze
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
    //ukryj menu
    this->setVisible(false);
    this->setEnabled(false);

    //stwórz nową grę
    game = new MainWindow(QString::fromStdString(plec), QString::fromStdString(imie), przeciwnik, this);
    game->show();
}

void Menu::on_ipButton_clicked()
{
    tcpSocket->connectToHost("127.0.0.1", port);
    cout << "Nawiązano połączenie" << endl;

}

void Menu::zeruj()
{
    //czysczenie danych rozgrywki
    pozycjeDrugiego[0]=0;
    pozycjeDrugiego[1]=0;
    pozycjeMoje[0]=0;
    pozycjeMoje[1]=0;
    wynik_1=0, wynik_2=0;

    odp_zamkniecie=false;
    odp_wygrana=-1;

}

void Menu::closeEvent(QCloseEvent *event)
{
    write(-1,"");
    cout << "Wychodzenie z gry..." << endl;
    qApp->closeAllWindows();
}
