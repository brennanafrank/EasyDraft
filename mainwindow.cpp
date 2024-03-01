#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "file_operations.hpp"


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

    ui->lineEdit->setVisible(false);

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



}


void MainWindow::on_actionTrash_2_triggered()
{

    QString currentDelete = ui->listWidget->currentItem()->text();

    std::string deleteStandard = currentDelete.toStdString();

    delete_template(deleteStandard);

    ui->listWidget->clear();

    if (!template_list().empty()) {

        for (int i = 0; i < template_list().size(); i++) {

            QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(template_list()[i]));
            ui->listWidget->addItem(item);

        }

    }

}


void MainWindow::on_actionDownload_2_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "...", QDir::homePath());

    std::filesystem::path name = path.toStdString();

    upload_template(name);

    ui->listWidget->clear();

    if (!template_list().empty()) {

        for (int i = 0; i < template_list().size(); i++) {

            QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(template_list()[i]));
            ui->listWidget->addItem(item);

        }

    }

}


void MainWindow::on_actionSearch_triggered()
{

    ui->lineEdit->setVisible(true);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::filterSearch);


}


void MainWindow::filterSearch(const QString &text) {

    for (int i = 0; i < ui->listWidget->count(); i++) {

        QListWidgetItem *item = ui->listWidget->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        ui->listWidget->setRowHidden(i, !matches);

    }

}

