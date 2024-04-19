#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE

#include <QDir>
#include "tagmanager.h"
#include "customfilesystemmodel.h"
#include "helpdialog.h"
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
#include <QPropertyAnimation>
#include <QColor>
#include <QVariantAnimation>
#include "replacementsHelper.h"
#include <QSettings>
#include <QTimer>
#include <QCryptographicHash>
#include <QListWidget>
#include <QMenu>
#include <QShortcut>





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
    void animateLineEditColorChange(QLineEdit* lineEdit, const QColor& startColor, const QColor& endColor, int duration = 500);
    void showContextMenuForPathViewer(const QPoint &pos);
    void updateTagComboBox();


private slots:
    void changePage();
    void on_actionBack_triggered();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_submitPlace_clicked();

    void on_cloudButton_clicked();

    void onCompleteFillButtonlicked();
    void onPageChanged(int page);
    void onFillFromJsonClicked(const QString &filePath);
    void onSaveDraftClicked();

    void on_selectFileToView_clicked();

    void on_fontButton_clicked();

    void on_ColorButton_clicked();

    void on_pathViewer_doubleClicked(const QModelIndex &index);

    void on_charMaxLimitButton_clicked();

    void saveSettings();

    void loadSettings();

    void onTagSelected(int index);

    void on_PDFButton_clicked();

    void on_TXTButton_clicked();

    void on_HTMLButton_clicked();

    // Auto-save draft
    void autoSaveDraft();
    void handleAutoSaveToggle(int state);
    void updateAutoSaveToggleState();
    QString hashFilePath(const QString& path);


    void on_back_button_export_view_clicked();

    void on_back_button_parsing_clicked();

    void on_back_button_view_doc_clicked();

    void on_importWaterMarkButton_clicked();

    void on_createWaterMarkButton_clicked();

    void on_pushButtonForWriting_clicked();

    void on_DocxButton_clicked();

    void on_printButton_clicked();

private:
    Ui::MainWindow *ui;
    void createFolder(const QModelIndex &index);
    void deleteAutoSaveAndTags(const QString&);
    void deleteItem();
    void addTag();
    void deleteTag();
    TagManager *tagManager;
    QListView* listView;
    QStringListModel* model;
    std::vector<std::string> filePathsForViewing;
    CustomFileSystemModel *fileSystemModel;


    std::vector<std::pair<std::string, std::vector<std::string>>> replacements;
    QSpinBox *pageSpinBox;
    int currentPageIndex; // Record the index before page value change for onPageChanged function
    int maxCharLimit;
    std::string docPath;
    QTimer* autoSaveTimer;


    //Create a vector of filePaths
    std::vector<QString> pathsofFiles;

    //Create a vector of filePaths to view if User wants to
    std::vector<std::string> viewPaths;

    std::string color;
    std::string fontSize;
    bool underline;
    bool italicize;
    bool bold;

    int typeOfExport;

    // This is for testing warning only; remove for later
    bool warning;

    bool dontAdd;

    QString writeWatermark;

    // tag filter
    void filterFilesByTag(const QString &tag); // Method to filter files
    bool filterIndexByTag(const QModelIndex &index, const QString &tag);
    void expandAllNodes(const QModelIndex &index);

    // search
    void searchFiles(const QString &searchQuery);
    bool searchIndex(const QModelIndex &index, const QString &searchQuery);

    // recent file
    QStringList recentFiles;
    void updateRecentFiles(const QString&);
    void updateRecentFilesList();
    void onRecentFileClicked(QListWidgetItem* item);
    void handleFileMoved(const QString &oldPath, const QString &newPath);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // Declare the override here
};

#endif // MAINWINDOW_H
