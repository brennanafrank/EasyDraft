#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE

#include <QDir>

#include "placeholdermanager.h"



namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();   

private slots:
    void changePage();
    void on_actionBack_triggered();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_5_clicked();

    void on_actionTrash_2_triggered();

    void on_actionDownload_2_triggered();

    void on_actionSearch_triggered();

    void filterSearch(const QString &);

    void onAddPlaceholderClicked();
    void onPlaceholderSelected(int index);
    void onDeletePlaceholderClicked();

private:
    Ui::MainWindow *ui;
    PlaceholderManager *placeholderManager;


};
#endif // MAINWINDOW_H
