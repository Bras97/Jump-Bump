#include "menu.h"
#include "ui_menu.h"
#include "mainwindow.h"
#include <iostream>
#include <QMessageBox>
#include <QtGui>

using namespace std;
string plec ="M";
string imie = "";


Menu::Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
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
    imie=ui->imieText->text().toStdString();
    if(imie=="")
    {
        QMessageBox::information(this,"Błąd", "Proszę podać imię królika!");
    }
    else
    {
        if(ui->plecM->isChecked()==false && ui->plecK->isChecked()==false)
            QMessageBox::information(this,"Błąd", "Proszę wybrać płeć królika!");
        else
        {
            if (ui->plecK->isChecked()==true)
                plec="K";
            ui->polaczButton->setEnabled(false);
            ui->imieText->setEnabled(false);
            ui->plecM->setEnabled(false);
            ui->plecK->setEnabled(false);
            ui->listaGraczy->setEnabled(true);
            ui->listaGraczy->setStyleSheet("background-color: white;");
            ui->odswiezButton->setEnabled(true);
            ui->listaGraczy->addItem("Jeriho");
            ui->listaGraczy->addItem("Havier");
            ui->listaGraczy->addItem("Konipia");

            MainWindow::writeLogin(imie);

        }
    }
}

void Menu::on_listaGraczy_clicked(const QModelIndex &index)
{
    ui->dolaczButton->setEnabled(true);
}


void Menu::on_dolaczButton_clicked()
{
    QMessageBox::information(this,"Informacja", "Wybrano królika: " + ui->listaGraczy->currentItem()->text());
    this->setVisible(false);
    this->setEnabled(false);
    this->close();
    MainWindow *game;
    game = new MainWindow(QString::fromStdString(plec), QString::fromStdString(imie), this);
    game->show();
}

