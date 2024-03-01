#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "file_operations.hpp""


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


    std::vector<std::string> templates = template_list();

    for (int i = 0;  i < templates.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(templates[i]));
        ui->listWidget->addItem(item);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePage() {

    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);

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


void MainWindow::on_actionTrash_2_triggered()
{



}


void MainWindow::on_actionDownload_2_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "...", QDir::homePath());

    std::filesystem::path name = path.toStdString();

    upload_template(name);

}

