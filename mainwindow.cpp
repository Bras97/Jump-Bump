#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QKeyEvent>

int skok=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QPixmap pix("img/forrest.jpg");
    //ui->background->setPixmap(pix);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left)
    {
        int x= ui->player_1->x()-3;
        ui->player_1->setGeometry(x,ui->player_1->y(),ui->player_1->width(),ui->player_1->height());
        //ui->player_1->setAttribute(Qt::WA_TranslucentBackground);
        //ui->player_1->repaint();
    }

    if(event->key() == Qt::Key_Up)
    {
        int y=0;
        if(skok<5)
        {
            y= ui->player_1->y()-(20-4*skok);
        }
        else
        {
            y= ui->player_1->y()+(4*skok-20);
        }
        if(skok<10) skok++;
        else skok=0;
        ui->player_1->setGeometry(ui->player_1->x(),y,ui->player_1->width(),ui->player_1->height());
        //ui->player_1->setAttribute(Qt::WA_TranslucentBackground);
        //ui->player_1->repaint();
    }
}
