#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "templates.h"

#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //delete ui->pushButton;
    //QPixmap pix("/Users/aneeshpendyala/Desktop/IMG_4864.jpeg");
    //ui->labelWelcome->setPixmap(pix);
    this->hide();

    Templates temp;
    temp.setModal(true);
    temp.exec();


}

