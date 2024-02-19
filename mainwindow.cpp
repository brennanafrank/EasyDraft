#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    treeView = new QTreeView(this);
    treeView->setModel(model);

    QPushButton *addFolderButton = new QPushButton("Add Folder", this);
    connect(addFolderButton, &QPushButton::clicked, this, &MainWindow::onAddFolderClicked);

    QPushButton *deleteFolderButton = new QPushButton("Delete Folder", this);
    connect(deleteFolderButton, &QPushButton::clicked, this, &MainWindow::onDeleteFolderClicked);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(treeView);
    layout->addWidget(addFolderButton);
    layout->addWidget(deleteFolderButton);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    loadModel();
}


MainWindow::~MainWindow()
{
    saveModel();
    delete ui;
}

void MainWindow::onAddFolderClicked() {
    QStandardItem *newFolder = new QStandardItem("New Folder");
    newFolder->setEditable(true);
    model->invisibleRootItem()->appendRow(newFolder);
}

void MainWindow::onDeleteFolderClicked() {
    QModelIndex currentIndex = treeView->currentIndex();
    if (!currentIndex.isValid())
        return;

    if (currentIndex.parent().isValid()) {
        model->itemFromIndex(currentIndex.parent())->removeRow(currentIndex.row());
    } else {
        model->removeRow(currentIndex.row());
    }
}


void MainWindow::saveModel() {
    QSettings settings("YourOrganization", "YourApp", this);
    settings.beginGroup("FolderStructure");
    settings.remove(""); // Clear existing settings

    for (int i = 0; i < model->invisibleRootItem()->rowCount(); ++i) {
        saveItem(settings, model->invisibleRootItem()->child(i), i);
    }
    settings.endGroup();
}

void MainWindow::saveItem(QSettings &settings, QStandardItem *item, int index) {
    if (!item) return;
    settings.beginGroup(QString::number(index));
    settings.setValue("name", item->text());
    settings.setValue("childCount", item->rowCount());
    for (int i = 0; i < item->rowCount(); ++i) {
        saveItem(settings, item->child(i), i);
    }
    settings.endGroup();
}


void MainWindow::loadModel() {
    QSettings settings("YourOrganization", "YourApp", this);
    settings.beginGroup("FolderStructure");
    model->clear();
    loadItem(settings, model->invisibleRootItem(), 0);
    settings.endGroup();
}

void MainWindow::loadItem(QSettings &settings, QStandardItem *parentItem, int index) {
    settings.beginGroup(QString::number(index));
    QString name = settings.value("name").toString();
    int childCount = settings.value("childCount", 0).toInt();

    QStandardItem *item = new QStandardItem(name);
    item->setEditable(true);

    if (parentItem != model->invisibleRootItem()) {
        parentItem->appendRow(item);
    } else {
        model->invisibleRootItem()->appendRow(item);
    }

    for (int i = 0; i < childCount; ++i) {
        loadItem(settings, item, i);
    }
    settings.endGroup();
}
