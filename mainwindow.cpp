#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customfilesystemmodel.h"
#include <QTreeView>
#include <QInputDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tagManager(new TagManager("/Users/michael/Downloads/Test_Folder/config.json"))
    , placeholderManager(new PlaceholderManager(this))
{
    ui->setupUi(this);

    placeholderManager->addDefaultPlaceholders();
    placeholderManager->loadPlaceholders();

    CustomFileSystemModel *fileSystemModel = new CustomFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());
    fileSystemModel->setTagManager(tagManager);
    ui->pathViewer->setModel(fileSystemModel);
    ui->pathViewer->setRootIndex(fileSystemModel->index("/Users/michael/Downloads/Test_Folder"));


    ui->pathViewer->setDragEnabled(true);
    ui->pathViewer->setAcceptDrops(true);
    ui->pathViewer->setDropIndicatorShown(true);

    connect(ui->createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(ui->deleteItemButton, &QPushButton::clicked, this, &MainWindow::deleteItem);
    connect(ui->deleteTagButton, &QPushButton::clicked, this, &MainWindow::deleteTag);
    connect(ui->addTagButton, &QPushButton::clicked, this, &MainWindow::onAddTagButtonClicked);


    connect(ui->addPlaceholderButton, &QPushButton::clicked, this, &MainWindow::onAddPlaceholderClicked);
    connect(ui->placeholderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlaceholderSelected(int)));
    connect(ui->deletePlaceholderButton, &QPushButton::clicked, this, &MainWindow::onDeletePlaceholderClicked);


    // Populate the combo box with existing placeholder names if any
    ui->placeholderComboBox->addItems(placeholderManager->getPlaceholderNames());

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

    QWidget *focusedWidget = QApplication::focusWidget();

    // Check if the selected placeholder is the current date and time
    if (placeholderName == "CurrentDateTime") {
        // Dynamically generate the current date and time
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        ui->lineEdit->setText(currentDateTime);
    } else {
        // if (focusedWidget && focusedWidget->metaObject()->className() == QLatin1String("QLineEdit")) {
        //     QLineEdit *lineEdit = static_cast<QLineEdit *>(focusedWidget);
        //     QString value = placeholderManager->getPlaceholderValue(placeholderName);
        //     lineEdit->setText(value);
        // }
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



MainWindow::~MainWindow()
{
    placeholderManager->savePlaceholders();
    delete ui;
    delete tagManager;
    delete placeholderManager;
}

