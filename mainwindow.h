#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tagmanager.h"
#include "placeholdermanager.h"
#include <QListView>
#include <QStringListModel>
#include <QLabel>
#include <QTreeView>
#include <QInputDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFormLayout>
#include <QSpinBox>
#include "parsing.h"

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

    void createDynamicPlaceholders(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);
    void clearWidgetsFromLayout(QLayout* layout);
    void updatePlaceholderValuesFromInputs();
    void updateCharCountLabel(QLineEdit* lineEdit, QLabel* charCountLabel);

private:
    Ui::MainWindow *ui;

    void createFolder();
    void deleteItem();
    void deleteTag();
    TagManager *tagManager;
    QListView* listView;
    QStringListModel* model;
    PlaceholderManager *placeholderManager;
    std::vector<std::pair<std::string, std::vector<std::string>>> replacements;
    QSpinBox *pageSpinBox;
protected:

private slots:
    void onAddTagButtonClicked();
    void onTagComboBoxCurrentIndexChanged(const QString &tag);
    void onAddPlaceholderClicked();
    void onPlaceholderSelected(int index);
    void onDeletePlaceholderClicked();
    void onCompleteFillButtonlicked();
    void onPageChanged(int page);
};
#endif // MAINWINDOW_H
