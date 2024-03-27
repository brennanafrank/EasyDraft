#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tagmanager.h"
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
#include <QFileDialog>

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

    void createDynamicPlaceholders(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacementsm, int maxCharLimit=20);
    void clearWidgetsFromLayout(QLayout* layout);
    void updateReplacementsFromInputs(int page);
    void updateCharCountLabel(QLineEdit* lineEdit, QLabel* charCountLabel);
    void updatePlaceholderValuesFromReplacements(int currentPage);

private:
    Ui::MainWindow *ui;

    void createFolder();
    void deleteItem();
    void deleteTag();
    TagManager *tagManager;
    QListView* listView;
    QStringListModel* model;

    std::vector<std::pair<std::string, std::vector<std::string>>> replacements;
    QSpinBox *pageSpinBox;
    int currentPageIndex; // Record the index before page value change for onPageChanged function
    std::string docPath;

protected:

private slots:
    void onAddTagButtonClicked();
    void onTagComboBoxCurrentIndexChanged(const QString &tag);

    void onCompleteFillButtonlicked();
    void onPageChanged(int page);
    void onFillFromJsonClicked();
    void onChooseDocPathClicked();
};
#endif // MAINWINDOW_H
