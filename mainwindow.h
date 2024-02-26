#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// #include "tagfilterproxymodel.h"
#include "tagmanager.h"

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

private:
    Ui::MainWindow *ui;
    void createFolder();
    void deleteItem();
    void deleteTag();
    TagManager *tagManager;
    // TagFilterProxyModel *tagFilterProxyModel;
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_addTagButton_clicked();
};
#endif // MAINWINDOW_H
