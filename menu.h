#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class Menu;
}

class Menu : public QDialog
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();

private slots:
    void on_plecM_clicked();

    void on_plecK_clicked();

    void on_polaczButton_clicked();

    void on_listaGraczy_clicked(const QModelIndex &index);

    void on_dolaczButton_clicked();

    QByteArray scalanie();

    void wyslij(std::string temp);

    void on_odswiezButton_clicked();
    void rozpocznijGre();

public slots:
    void write(int polecenie, std::string imie);
    void readData();

private:
    Ui::Menu *ui;
    QTcpSocket *tcpSocket;
};

#endif // MENU_H
