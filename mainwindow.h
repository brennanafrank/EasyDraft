#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QPushButton>
#include <QTreeView>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddFolderClicked();
    void onDeleteFolderClicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QTreeView *treeView;


    void saveModel();
    void loadModel();
    void saveItem(QSettings &settings, QStandardItem *item, int index);
    void loadItem(QSettings &settings, QStandardItem *parentItem, int index);
};
#endif // MAINWINDOW_H
