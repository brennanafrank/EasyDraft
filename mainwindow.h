#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE

#include <QDir>
#include "tagmanager.h"
#include "customfilesystemmodel.h"
#include <QListView>
#include <QStringListModel>
#include <vector>
#include <string>

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


private slots:
    void changePage();
    void on_actionBack_triggered();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_actionTrash_2_triggered();

    void on_actionDownload_2_triggered();

    void on_actionSearch_triggered();

    void filterSearch(const QString &);

    void onAddTagButtonClicked();

    void onTagComboBoxCurrentIndexChanged(const QString &tag);

    void on_actionAscending_triggered();

    void on_actionDescending_triggered();

    void on_pushButton_6_clicked();

    void on_pushButton_clicked();

    void on_submitPlace_clicked();

    void on_cloudButton_clicked();

    void onCompleteFillButtonlicked();
    void onPageChanged(int page);
    void onFillFromJsonClicked();
    void onChooseDocPathClicked();
    void onSaveDraftClicked();

private:
    Ui::MainWindow *ui;
    void createFolder();
    void deleteItem();
    void deleteTag();
    TagManager *tagManager;
    QListView* listView;
    QStringListModel* model;
    std::vector<std::string> filePathsForViewing;

    std::vector<std::pair<std::string, std::vector<std::string>>> replacements;
    QSpinBox *pageSpinBox;
    int currentPageIndex; // Record the index before page value change for onPageChanged function
    std::string docPath;



    // This is for testing warning only; remove for later
    bool warning;


};
#endif // MAINWINDOW_H
