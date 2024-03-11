#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tagmanager.h"
#include "placeholdermanager.h"
#include <QListView>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
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

    void createDynamicPlaceholders(const std::vector<std::pair<std::string, std::string>>& replacements);
    void clearWidgetsFromLayout(QLayout* layout);
    void updatePlaceholderValuesFromInputs();


private:
    Ui::MainWindow *ui;

    void createFolder();
    void deleteItem();
    void deleteTag();
    TagManager *tagManager;
    QListView* listView;
    QStringListModel* model;
    PlaceholderManager *placeholderManager;
    std::vector<std::pair<std::string, std::string>> replacements;

protected:

private slots:
    void onAddTagButtonClicked();
    void onTagComboBoxCurrentIndexChanged(const QString &tag);
    void onAddPlaceholderClicked();
    void onPlaceholderSelected(int index);
    void onDeletePlaceholderClicked();
    void onCompleteFillButtonlicked();
};
#endif // MAINWINDOW_H
