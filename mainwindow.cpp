#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "file_operations.hpp"

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QTreeView>



// Q entry list
// Q list widget

//  Upload the file and return the file



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tagManager(new TagManager("/Users/aneeshpendyala/Desktop/Hellodjwqdjwoiq/config.json"))
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

    CustomFileSystemModel *fileSystemModel = new CustomFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());
    fileSystemModel->setTagManager(tagManager);
    ui->pathViewer->setModel(fileSystemModel);
    ui->pathViewer->setRootIndex(fileSystemModel->index("/Users/aneeshpendyala/Desktop/Hellodjwqdjwoiq"));


    ui->pathViewer->setDragEnabled(true);
    ui->pathViewer->setAcceptDrops(true);
    ui->pathViewer->setDropIndicatorShown(true);

    connect(ui->addPlaceholderButton, &QPushButton::clicked, this, &MainWindow::onAddPlaceholderClicked);
    connect(ui->placeholderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlaceholderSelected(int)));
    connect(ui->deletePlaceholderButton, &QPushButton::clicked, this, &MainWindow::onDeletePlaceholderClicked);
    connect(ui->createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(ui->deleteItemButton, &QPushButton::clicked, this, &MainWindow::deleteItem);
    connect(ui->deleteTagButton, &QPushButton::clicked, this, &MainWindow::deleteTag);
    connect(ui->addTagButton, &QPushButton::clicked, this, &MainWindow::onAddTagButtonClicked);
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


void MainWindow::onTagComboBoxCurrentIndexChanged(const QString &tag) {
    auto *model = static_cast<CustomFileSystemModel*>(ui->pathViewer->model());
    model->setFilterTag(tag);
}



void MainWindow::createFolder() {
    // Get the current chosen path
    QModelIndex index = ui->pathViewer->currentIndex();
    if (!index.isValid()) return;

    // convert the model into FileSystemModel
    QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
    QString itemPath = model->filePath(index);
    QFileInfo fileInfo(itemPath);

    // Determine whether it is directory
    QString targetPath;
    if (fileInfo.isDir()) {
        targetPath = itemPath;
    } else {
        targetPath = fileInfo.path();
    }

    // Get new file name from user
    QString folderName = QInputDialog::getText(this, tr("Create Folder"), tr("Folder Name:"));
    if (folderName.isEmpty()) return;

    // Construct the folderpath
    QString folderPath = QDir(targetPath).absoluteFilePath(folderName);

    // attempt to create folder
    if (!QDir().mkdir(folderPath)) {
        QMessageBox::warning(this, tr("Create Folder"), tr("Failed to create folder at %1.").arg(folderPath));
    } else {
        QMessageBox::information(this, tr("Create Folder"), tr("Folder created successfully."));
    }

}

void MainWindow::deleteItem() {
    QModelIndex index = ui->pathViewer->currentIndex();
    if (!index.isValid()) return;

    if (QMessageBox::question(this, tr("Delete Item"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
        if (!model->remove(index)) {
            QMessageBox::warning(this, tr("Delete Item"), tr("Failed to delete item."));
        }
    }
}

QString MainWindow::getCurrentSelectedFilePath() {
    QModelIndex currentIndex = ui->pathViewer->currentIndex();
    if (!currentIndex.isValid()) {
        return QString();
    }

    QFileSystemModel *model = static_cast<QFileSystemModel*>(ui->pathViewer->model());
    return model->filePath(currentIndex);
}


void MainWindow::onAddTagButtonClicked() {
    QString filePath = getCurrentSelectedFilePath();
    if (filePath.isEmpty()) {
        return;
    }

    bool ok;
    QString tag = QInputDialog::getText(this, tr("Add Tag"),
                                        tr("Tag:"), QLineEdit::Normal,
                                        QDir::home().dirName(), &ok);
    if (ok && !tag.isEmpty()) {
        tagManager->addTag(filePath, tag);
    }
}

void MainWindow::deleteTag() {
    QString filePath = getCurrentSelectedFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a file first."));
        return;
    }

    QStringList tags = tagManager->getTags(filePath);
    if (tags.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("Selected file has no tags to delete."));
        return;
    }

    bool ok;
    QString tagToDelete = QInputDialog::getItem(this, tr("Delete Tag"),
                                                tr("Select a tag to delete:"), tags, 0, false, &ok);
    if (ok && !tagToDelete.isEmpty()) {
        tagManager->removeTag(filePath, tagToDelete);
        QMessageBox::information(this, tr("Info"), tr("Tag deleted successfully."));

        // Optionally refresh the view if necessary
    }
}


