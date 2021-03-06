#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QApplication>
#include <QLabel>
#include <QtNetwork>
#include <string>
#include <iostream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &plec, const QString &imie, const QString &przeciwnik,  QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void detekcja(QLabel *blok);
    bool kolizja(int poz0, int poz1,int poz2,int poz3);
//    bool zderzenie(int poz0, int poz1,int poz2,int poz3, int pozd0, int pozd1,int pozd2,int pozd3);
    void koniec_gry(QString napis);

public slots:
    void ruch();
    void closeEvent(QCloseEvent *event);

//    void displayError(QAbstractSocket::SocketError socketError);


private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTcpSocket *tcpSocket;
    QPixmap rabbit_l1, rabbit_l2, rabbit_r1, rabbit_r2, rabbit2_l1, rabbit2_l2, rabbit2_r1, rabbit2_r2;

};

#endif // MAINWINDOW_H
