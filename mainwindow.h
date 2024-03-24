#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE

#include <QDir>

#include "placeholdermanager.h"
#include "tagmanager.h"
#include "customfilesystemmodel.h"
#include <QListView>
#include <QStringListModel>
#include <vector>
#include <string>



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
    QString getCurrentSelectedFilePath();
    void displayFilesForTag(const QString &tag);


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

    void onAddTagButtonClicked();

    void onTagComboBoxCurrentIndexChanged(const QString &tag);



    void on_actionAscending_triggered();

    void on_actionDescending_triggered();

    void on_pushButton_6_clicked();


    void on_pushButton_clicked();

    void checkChange(const QString &text);

private:
    Ui::MainWindow *ui;
    PlaceholderManager *placeholderManager;
    void createFolder();
    void deleteItem();
    void deleteTag();
    TagManager *tagManager;
    QListView* listView;
    QStringListModel* model;
    std::vector<std::string> filePathsForViewing;



    // This is for testing warning only; remove for later
    bool warning;


};
#endif // MAINWINDOW_H
