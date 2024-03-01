#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "file_operations.hpp"

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

// Q entry list
// Q list widget

//  Upload the file and return the file



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , placeholderManager(new PlaceholderManager(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);


    std::vector<std::string> templates = template_list();

    for (int i = 0;  i < templates.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(templates[i]));
        ui->listWidget->addItem(item);
    }

    ui->lineEdit_2->setVisible(false);


    placeholderManager->addDefaultPlaceholders();
    placeholderManager->loadPlaceholders();

    connect(ui->addPlaceholderButton, &QPushButton::clicked, this, &MainWindow::onAddPlaceholderClicked);
    connect(ui->placeholderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlaceholderSelected(int)));
    connect(ui->deletePlaceholderButton, &QPushButton::clicked, this, &MainWindow::onDeletePlaceholderClicked);

    // Populate the combo box with existing placeholder names if any
    ui->placeholderComboBox->addItems(placeholderManager->getPlaceholderNames());

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

    ui->lineEdit_2->setVisible(true);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::filterSearch);


}


void MainWindow::filterSearch(const QString &text) {

    for (int i = 0; i < ui->listWidget->count(); i++) {

        QListWidgetItem *item = ui->listWidget->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        ui->listWidget->setRowHidden(i, !matches);

    }

}



void MainWindow::onAddPlaceholderClicked() {
    bool ok;
    // Prompt user for placeholder name
    QString placeholderName = QInputDialog::getText(this, tr("Add Placeholder"),
                                                    tr("Placeholder Name:"), QLineEdit::Normal,
                                                    QString(), &ok);
    if (ok && !placeholderName.isEmpty()) {
        // Prompt user for placeholder value
        QString placeholderValue = QInputDialog::getText(this, tr("Placeholder Value"),
                                                         tr("Value for ") + placeholderName + ":", QLineEdit::Normal,
                                                         QString(), &ok);
        if (ok) {
            // Use PlaceholderManager to add placeholder
            placeholderManager->addPlaceholder(placeholderName, placeholderValue);
            placeholderManager->savePlaceholders();
            // Update UI ComboBox
            ui->placeholderComboBox->addItem(placeholderName);
        }
    }
}


void MainWindow::onPlaceholderSelected(int index) {
    if (index < 0) return; // Ensure valid index

    QString placeholderName = ui->placeholderComboBox->itemText(index);

    // Check if the selected placeholder is the current date and time
    if (placeholderName == "CurrentDateTime") {
        // Dynamically generate the current date and time
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        ui->lineEdit->setText(currentDateTime);
    } else {
        // For other placeholders, fetch the stored value
        QString value = placeholderManager->getPlaceholderValue(placeholderName);
        ui->lineEdit->setText(value);
    }
}

void MainWindow::onDeletePlaceholderClicked() {
    QString placeholderName = ui->placeholderComboBox->currentText();
    if (!placeholderName.isEmpty()) {
        placeholderManager->removePlaceholder(placeholderName);
        ui->placeholderComboBox->removeItem(ui->placeholderComboBox->currentIndex());

        // Optionally, save placeholders to persist the deletion
        placeholderManager->savePlaceholders();
    }
}


