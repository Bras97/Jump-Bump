#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QApplication>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &plec, const QString &imie, QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void detekcja(QLabel *blok);
    bool kolizja(int poz0, int poz1,int poz2,int poz3);
    bool zderzenie(int poz0, int poz1,int poz2,int poz3, int pozd0, int pozd1,int pozd2,int pozd3);
    void koniec_gry();
public slots:
    void ruch();


private:
    Ui::MainWindow *ui;
    QTimer *timer;

};

#endif // MAINWINDOW_H
