#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileSystemModel>
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
{
    ui->setupUi(this);

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    ui->pathViewer->setModel(model);
    ui->pathViewer->setRootIndex(model->index("/Users/michael/Downloads"));

    connect(ui->createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(ui->deleteItemButton, &QPushButton::clicked, this, &MainWindow::deleteItem);

}

void MainWindow::createFolder() {

    QModelIndex index = ui->pathViewer->currentIndex();
    if (!index.isValid()) return;

    QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
    QString itemPath = model->filePath(index);
    QFileInfo fileInfo(itemPath);

    QString targetPath;
    if (fileInfo.isDir()) {
        targetPath = itemPath;
    } else {
        targetPath = fileInfo.path();
    }

    QString folderName = QInputDialog::getText(this, tr("Create Folder"), tr("Folder Name:"));
    if (folderName.isEmpty()) return;

    QString folderPath = QDir(targetPath).absoluteFilePath(folderName);

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

MainWindow::~MainWindow()
{
    delete ui;
}
