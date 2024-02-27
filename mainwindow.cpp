#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

// Q entry list
// Q list widget

//  Upload the file and return the file



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePage() {

    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);

}

/*QDir MainWindow:: constructPath() {

    QString path = QFileDialog::getExistingDirectory(nullptr, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly);


    // Construct a file path to the directory using QDir

    QDir userDir(path);

    // Error checking if the path to the directory exists

    if (!userDir.exists()) {

        qWarning() << "Path not found" << path;
        throw in

    }


    return userDir;


}*/


void MainWindow::on_pushButton_clicked()
{
    changePage();

}


void MainWindow::on_pushButton_2_clicked()
{

    // Get the directory of the user

    // Set the parent to null
    //Caption = "Select Directory"
    //Initial directory is home
    // Filter is directorys only
    QString path = QFileDialog::getExistingDirectory(nullptr, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly);


    // Construct a file path to the directory using QDir

    QDir userDir(path);

    // Error checking if the path to the directory exists

    if (!userDir.exists()) {

        qWarning() << "Path not found" << path;
        exit(1);

    }

    else {

        // Based on stack overflow page in how to get files from the directories

        for (const QFileInfo &file : userDir.entryInfoList(QDir::Files))
        {
            QListWidgetItem *item = new QListWidgetItem(file.fileName());
            item->setData(Qt::UserRole, file.absolutePath()); // if you need absolute path of the file
            ui->listWidget->addItem(item);
        }

    }


}


void MainWindow::on_actionBack_triggered()
{

    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);

}


void MainWindow::on_pushButton_4_clicked()
{

    if (ui->listWidget->count() == 0) {

        QMessageBox::warning(nullptr, "Warning", "No documents!");

    }
    else {

        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);

        ui->label->setText(ui->listWidget->currentItem()->text());

    }

}


void MainWindow::on_pushButton_3_clicked()
{

    changePage();

}


void MainWindow::on_pushButton_8_clicked()
{
    changePage();

}


void MainWindow::on_pushButton_5_clicked()
{

    QString path = QFileDialog::getExistingDirectory(nullptr, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly);


    // Construct a file path to the directory using QDir

    QDir userDir(path);

    // Error checking if the path to the directory exists

    if (!userDir.exists()) {

        qWarning() << "Path not found" << path;
        exit(1);

    }

}

