#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QApplication>
#include <QLabel>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void detekcja(QLabel *blok);
    bool kolizja(int poz0, int poz1,int poz2,int poz3);
    bool zderzenie(int poz0, int poz1,int poz2,int poz3, int pozd0, int pozd1,int pozd2,int pozd3);
public slots:
    void ruch();
    void write();
    void readData();
//    void displayError(QAbstractSocket::SocketError socketError);


private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTcpSocket *tcpSocket;

};

#endif // MAINWINDOW_H
