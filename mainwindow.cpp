#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "file_operations.hpp"
#include "cloud.hpp"

#include <fstream>


#include <QVBoxLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QPixmap>
#include <QTreeView>



// Q entry list
// Q list widget
//  Upload the file and return the file



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tagManager(new TagManager("/Users/aneeshpendyala/Desktop/Json Directory/config.json"))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);


    std::vector<std::string> templates = template_list();
    for (int i = 0;  i < templates.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(templates[i]));
        ui->listWidget->addItem(item);
    }
    ui->lineEdit_2->setVisible(false);

    // Allow for sorting

    ui->listWidget->setSortingEnabled(true);



    CustomFileSystemModel *fileSystemModel = new CustomFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());
    fileSystemModel->setTagManager(tagManager);
    ui->pathViewer->setModel(fileSystemModel);
    ui->pathViewer->setRootIndex(fileSystemModel->index("/Users/aneeshpendyala/Desktop/Json Directory"));


    ui->pathViewer->setDragEnabled(true);
    ui->pathViewer->setAcceptDrops(true);
    ui->pathViewer->setDropIndicatorShown(true);


    connect(ui->createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(ui->deleteItemButton, &QPushButton::clicked, this, &MainWindow::deleteItem);
    connect(ui->deleteTagButton, &QPushButton::clicked, this, &MainWindow::deleteTag);
    connect(ui->addTagButton, &QPushButton::clicked, this, &MainWindow::onAddTagButtonClicked);

    connect(ui->fillFromJsonButton, &QPushButton::clicked, this, &MainWindow::onFillFromJsonClicked);
    connect(ui->chooseDocPathButton, &QPushButton::clicked, this, &MainWindow::onChooseDocPathClicked);


    // Setting up all the fonts that a user can select
    for (int j = 1; j <= 12; ++j) {

        ui->fontSelector->addItem(QString::fromStdString(std::to_string(j)));

    }

    ui->ColorSelector->addItem("Red");
    ui->ColorSelector->addItem("Blue");
    ui->ColorSelector->addItem("Yellow");
    ui->ColorSelector->addItem("Pink");
    ui->ColorSelector->addItem("Purple");
    ui->ColorSelector->addItem("Green");
    ui->ColorSelector->addItem("Black");
    ui->ColorSelector->addItem("White");


    // Highlighting the name of the file we're at

    QFont font("Arial", 20, QFont::Bold);

    ui->label->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePage() {

    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);

}

void MainWindow::on_actionBack_triggered()
{

    if (ui->stackedWidget->currentIndex() == 4) {

        ui->stackedWidget->setCurrentWidget(ui->page_4);

    }
    else {

        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);

    }

    ui->textBrowser->setVisible(false);
    ui->viewPathsListWidget->setVisible(true);

}


void MainWindow::on_pushButton_4_clicked()
{

    if (ui->listWidget->count() == 0) {

        QMessageBox::warning(nullptr, "Warning", "No documents!");

    }
    else {

        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);

        ui->label->setText(ui->listWidget->currentItem()->text());

        std::ifstream file("paths.txt");

        if (file.is_open()) {

            std::string line;

            while (std::getline(file, line)) {

                pathsofFiles.push_back(QString::fromStdString(line));

            }

            file.close();

        }



        QString filePath;

        for (int i = 0; i < pathsofFiles.size(); ++i) {

            if (pathsofFiles[i].contains(ui->listWidget->currentItem()->text())) {

                filePath = pathsofFiles[i];

            }

        }

        docPath = filePath.toStdString();
        replacements = findPlaceholdersInDocument(docPath);
        createDynamicPlaceholders(replacements);
        QMessageBox::information(this, tr("Document Loaded"), tr("Placeholders loaded from the selected document."));

    }

}


void MainWindow::on_pushButton_3_clicked()
{

    ui->stackedWidget->setCurrentWidget(ui->page_5);

}


void MainWindow::on_pushButton_5_clicked()
{

    QString currentUpdate = ui->listWidget->currentItem()->text();

    std::string updateStandard = currentUpdate.toStdString();

    int error = export_document(updateStandard);

    if (error < 0) {

        QString status = QString("Error(%d)").arg(error);

        QMessageBox::about(this, "Warning", status);

    }

}


void MainWindow::on_actionTrash_2_triggered()
{

    QString currentDelete = ui->listWidget->currentItem()->text();

    std::string deleteStandard = currentDelete.toStdString();

    delete_template(deleteStandard);

    ui->listWidget->clear();

    if (!template_list().empty()) {

        for (int i = 0; i < template_list().size(); i++) {

            QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(template_list()[i]));
            ui->listWidget->addItem(item);

        }

    }

}


void MainWindow::on_actionDownload_2_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "...", QDir::homePath());

    std::filesystem::path name = path.toStdString();

    upload_template(name);

    ui->listWidget->clear();

    if (!template_list().empty()) {

        for (int i = 0; i < template_list().size(); i++) {

            QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(template_list()[i]));
            ui->listWidget->addItem(item);

        }

    }

}


void MainWindow::on_actionSearch_triggered()
{

    ui->lineEdit_2->setVisible(true);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::filterSearch);


}


void MainWindow::filterSearch(const QString &text) {

    for (int i = 0; i < ui->listWidget->count(); i++) {

        QListWidgetItem *item = ui->listWidget->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        ui->listWidget->setRowHidden(i, !matches);

    }

}


void MainWindow::onTagComboBoxCurrentIndexChanged(const QString &tag) {
    auto *model = static_cast<CustomFileSystemModel*>(ui->pathViewer->model());
    model->setFilterTag(tag);
}



void MainWindow::createFolder() {
    // Get the current chosen path
    QModelIndex index = ui->pathViewer->currentIndex();
    if (!index.isValid()) return;

    // convert the model into FileSystemModel
    QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
    QString itemPath = model->filePath(index);
    QFileInfo fileInfo(itemPath);

    // Determine whether it is directory
    QString targetPath;
    if (fileInfo.isDir()) {
        targetPath = itemPath;
    } else {
        targetPath = fileInfo.path();
    }

    // Get new file name from user
    QString folderName = QInputDialog::getText(this, tr("Create Folder"), tr("Folder Name:"));
    if (folderName.isEmpty()) return;

    // Construct the folderpath
    QString folderPath = QDir(targetPath).absoluteFilePath(folderName);

    // attempt to create folder
    if (!QDir().mkdir(folderPath)) {
        QMessageBox::warning(this, tr("Create Folder"), tr("Failed to create folder at %1.").arg(folderPath));
    } else {
        QMessageBox::information(this, tr("Create Folder"), tr("Folder created successfully."));
    }

}

void MainWindow::deleteItem() {
    QModelIndex index = ui->pathViewer->currentIndex();
    if (!index.isValid()) return;

    if (QMessageBox::question(this, tr("Delete Item"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
        if (!model->remove(index)) {
            QMessageBox::warning(this, tr("Delete Item"), tr("Failed to delete item."));
        }
    }
}

QString MainWindow::getCurrentSelectedFilePath() {
    QModelIndex currentIndex = ui->pathViewer->currentIndex();
    if (!currentIndex.isValid()) {
        return QString();
    }

    QFileSystemModel *model = static_cast<QFileSystemModel*>(ui->pathViewer->model());
    return model->filePath(currentIndex);
}


void MainWindow::onAddTagButtonClicked() {
    QString filePath = getCurrentSelectedFilePath();
    if (filePath.isEmpty()) {
        return;
    }

    bool ok;
    QString tag = QInputDialog::getText(this, tr("Add Tag"),
                                        tr("Tag:"), QLineEdit::Normal,
                                        QDir::home().dirName(), &ok);
    if (ok && !tag.isEmpty()) {
        tagManager->addTag(filePath, tag);
    }
}

void MainWindow::deleteTag() {
    QString filePath = getCurrentSelectedFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a file first."));
        return;
    }

    QStringList tags = tagManager->getTags(filePath);
    if (tags.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("Selected file has no tags to delete."));
        return;
    }

    bool ok;
    QString tagToDelete = QInputDialog::getItem(this, tr("Delete Tag"),
                                                tr("Select a tag to delete:"), tags, 0, false, &ok);
    if (ok && !tagToDelete.isEmpty()) {
        tagManager->removeTag(filePath, tagToDelete);
        QMessageBox::information(this, tr("Info"), tr("Tag deleted successfully."));

        // Optionally refresh the view if necessary
    }
}


// When ascending is triggered

void MainWindow::on_actionAscending_triggered()
{
    ui->listWidget->sortItems(Qt::AscendingOrder);

}


//Descnding triggered
void MainWindow::on_actionDescending_triggered()
{

    ui->listWidget->sortItems(Qt::DescendingOrder);

}


void MainWindow::on_pushButton_6_clicked()
{


    std::ifstream file("savedpaths.txt");

    if (file.is_open()) {

        std::string line;

        while (std::getline(file, line)) {

            viewPaths.push_back(line);

        }

        file.close();

    }
    // This is a work in progress code for reading a docx file

    ui->textBrowser->setVisible(false);

    for (int i = 0; i < viewPaths.size(); ++i) {

        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(viewPaths[i]));

        ui->viewPathsListWidget->addItem(item);

    }

    changePage();

}

void MainWindow::on_pushButton_clicked()
{

    if (!ui->warningLine->text().isEmpty()) {

        warning = true;

    }

}

void MainWindow::on_submitPlace_clicked()
{

    if (warning) {

        changePage();

    }
    else {

        QMessageBox::critical(nullptr, "Error", "Text cannot be empty.");
        ui->warningLine->setStyleSheet("QLineEdit { background-color: red; }");

    }

}



void MainWindow::on_cloudButton_clicked()
{
    int a = GD_create_easydraft();
}

void MainWindow::createDynamicPlaceholders(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacementsm, int maxCharLimit) {
    QWidget* contents = ui->scrollAreaWidgetContents;
    QFormLayout* layout = qobject_cast<QFormLayout*>(contents->layout());
    // Create a horizontal layout for the buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;

    QPushButton* saveDraftButton = new QPushButton("Save Draft", ui->scrollAreaWidgetContents);
    connect(saveDraftButton, &QPushButton::clicked, this, &MainWindow::onSaveDraftClicked);
    buttonLayout->addWidget(saveDraftButton);

    QPushButton* completeFillButton = new QPushButton("Complete", ui->scrollAreaWidgetContents);
    connect(completeFillButton, &QPushButton::clicked, this, &MainWindow::onCompleteFillButtonlicked);
    buttonLayout->addWidget(completeFillButton);

    // If the layout already exists, clear all widgets from it
    if (layout) {
        clearWidgetsFromLayout(layout);
    } else {
        // If no layout exists, create a new one
        layout = new QFormLayout(contents);
        contents->setLayout(layout);
    }

    // debug
    // for (const auto& pair : replacements) {
    //     qDebug() << "Key:" << QString::fromStdString(pair.first);
    //     qDebug() << "Values (type:" << QString::fromStdString(typeid(pair.second).name()) << ")";
    //     for (const auto& value : pair.second) {
    //         qDebug() << QString::fromStdString(value);
    //     }
    // }

    int keyIndex = 0;
    for (const auto& pair : replacements) {
        QLabel* label = new QLabel(QString::fromStdString(pair.first));
        QLineEdit* lineEdit = new QLineEdit(); // No initial text, will be set by onPageChanged
        lineEdit->setMaxLength(maxCharLimit);
        // if (!pair.second.empty()) {
        //     lineEdit->setText(QString::fromStdString(pair.second[0]));
        // }

        // Character count label
        QLabel* charCountLabel = new QLabel(QString("0/%1").arg(maxCharLimit));
        updateCharCountLabel(lineEdit, charCountLabel); // Initial update

        // Connect the textChanged signal to update the character count dynamically
        connect(lineEdit, &QLineEdit::textChanged, this, [this, lineEdit, charCountLabel, maxCharLimit]() {
            updateCharCountLabel(lineEdit, charCountLabel);
        });

        QHBoxLayout* rowLayout = new QHBoxLayout();
        rowLayout->addWidget(lineEdit);
        rowLayout->addWidget(charCountLabel);

        layout->addRow(label, rowLayout);
    }

    // Add page label and spin box
    QLabel* pageLabel = new QLabel("Page:");
    pageSpinBox = new QSpinBox(contents);
    pageSpinBox->setMinimum(1);
    connect(pageSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onPageChanged);

    QHBoxLayout* pageLayout = new QHBoxLayout();
    pageLayout->addWidget(pageLabel);


    pageLayout->addWidget(pageSpinBox);
    layout->addRow(pageLayout);

    layout->addRow(buttonLayout);
}



void MainWindow::onPageChanged(int newPage) {
    QFormLayout* layout = qobject_cast<QFormLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!layout) return;

    for (auto& pair : replacements) {
        pair.second.resize(std::max(pair.second.size(), static_cast<size_t>(newPage)));
    }
    qDebug() << "test";
    updateReplacementsFromInputs(currentPageIndex);
    currentPageIndex = pageSpinBox->value();
    qDebug() << "test2";

    // for (int i = 0; i < replacements.size(); ++i) {
    //     QHBoxLayout* rowLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(i, QFormLayout::FieldRole)->layout());
    //     if (rowLayout && !rowLayout->isEmpty()) {
    //         QLineEdit* lineEdit = qobject_cast<QLineEdit*>(rowLayout->itemAt(0)->widget());
    //         if (lineEdit) {
    //             lineEdit->setText(QString::fromStdString(replacements[i].second[newPage - 1]));
    //         }
    //     }
    // }
    updatePlaceholderValuesFromReplacements(currentPageIndex);
    qDebug() << "test3";
}


void MainWindow::clearWidgetsFromLayout(QLayout* layout) {
    QFormLayout* formLayout = qobject_cast<QFormLayout*>(layout);
    if (!formLayout) return;

    // Removing widgets from the form layout
    while (formLayout->rowCount() > 0) {
        QLayoutItem* item = formLayout->itemAt(0, QFormLayout::LabelRole);
        if (item && item->widget()) {
            item->widget()->deleteLater();
        }
        item = formLayout->itemAt(0, QFormLayout::FieldRole);
        if (item && item->widget()) {
            item->widget()->deleteLater();
        }
        formLayout->removeRow(0);
    }
}

void MainWindow::updateCharCountLabel(QLineEdit* lineEdit, QLabel* charCountLabel) {
    int currentLength = lineEdit->text().length();
    int maxLength = lineEdit->maxLength();
    charCountLabel->setText(QString("%1/%2").arg(currentLength).arg(maxLength));
}





void MainWindow::updateReplacementsFromInputs(int currentPage) {
    QFormLayout* layout = qobject_cast<QFormLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!layout) return;

    // Resize replacements vectors to accommodate new page data if needed
    for (auto& pair : replacements) {
        pair.second.resize(std::max(pair.second.size(), static_cast<size_t>(currentPage)));
    }

    // Update replacements with the new values
    for (int i = 0; i < layout->rowCount() - 2; ++i) {
        QLabel* label = qobject_cast<QLabel*>(layout->itemAt(i, QFormLayout::LabelRole)->widget());
        QHBoxLayout* rowLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(i, QFormLayout::FieldRole)->layout());
        if (rowLayout && !rowLayout->isEmpty() && label) {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(rowLayout->itemAt(0)->widget());
            if (lineEdit) {
                replacements[i].second[currentPage - 1] = lineEdit->text().toStdString();
            }
        }
    }

    // for (const auto& pair : replacements) {
    //     qDebug() << "Key:" << QString::fromStdString(pair.first);
    //     qDebug() << "Values:";
    //     for (const auto& value : pair.second) {
    //         qDebug() << QString::fromStdString(value);
    //     }
    // }
}

void MainWindow::updatePlaceholderValuesFromReplacements(int currentPage) {
    QFormLayout* layout = qobject_cast<QFormLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!layout) return;

    // Resize replacements vectors to accommodate new page data if needed
    for (auto& pair : replacements) {
        pair.second.resize(std::max(pair.second.size(), static_cast<size_t>(currentPage)));
    }

    // Update QLineEdit widgets with the values from replacements
    for (int i = 0; i < layout->rowCount() - 2; ++i) {
        QHBoxLayout* rowLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(i, QFormLayout::FieldRole)->layout());
        if (rowLayout && !rowLayout->isEmpty()) {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(rowLayout->itemAt(0)->widget());
            if (lineEdit) {
                if (currentPage - 1 < replacements[i].second.size()) {
                    lineEdit->setText(QString::fromStdString(replacements[i].second[currentPage - 1]));
                } else {
                    lineEdit->clear();
                }
            }
        }
    }
}



void MainWindow::onCompleteFillButtonlicked() {
    updateReplacementsFromInputs(pageSpinBox->value());

    // Ask the user for the save path
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Save Directory"), QDir::homePath());
    if (dirPath.isEmpty()) {
        return;
    }

    // Ask the user for the file name prefix
    bool ok;
    QString prefix = QInputDialog::getText(this, tr("File Name Prefix"), tr("Enter the file name prefix:"), QLineEdit::Normal, "modified", &ok);
    if (!ok || prefix.isEmpty()) {
        return;
    }

    qDebug() << fontSize;

    modifyDocument(docPath, vectorToJson(replacements), dirPath, prefix, color, fontSize);


    // Ask the user if they want to save the placeholder values to a JSON file
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Save Placeholder Values",
                                                              "Do you want to save the placeholder values to a JSON file for future use?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QString selectedFilter;
        QString filePath = QFileDialog::getSaveFileName(this, "Save JSON File", QDir::homePath(), "JSON Files (*.json)", &selectedFilter);

        if (!filePath.isEmpty()) {
            try {
                saveJsonToFile(replacements, filePath.toStdString());
                QMessageBox::information(this, "Success", "Placeholder values saved to " + filePath);
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
            }
        }
    }
}



void MainWindow::onFillFromJsonClicked()
{
    // check whether docPath is empty
    if (docPath.empty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a document first."));
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, tr("Open JSON File"), QDir::homePath(), tr("JSON Files (*.json)"));
    if (!filePath.isEmpty()) {
        try {
            std::vector<std::pair<std::string, std::vector<std::string>>> jsonReplacements = readJsonFromFile(filePath.toStdString());

            // Check if the keys in the JSON file match the keys in replacements
            std::vector<std::string> jsonKeys, replacementsKeys;
            for (const auto& pair : jsonReplacements) {
                jsonKeys.push_back(pair.first);
            }
            for (const auto& pair : replacements) {
                replacementsKeys.push_back(pair.first);
            }

            std::sort(jsonKeys.begin(), jsonKeys.end());
            std::sort(replacementsKeys.begin(), replacementsKeys.end());

            if (jsonKeys != replacementsKeys) {
                QMessageBox::warning(this, tr("Error"), tr("The keys in the JSON file do not match the placeholders in the document."));
                return;
            }

            // If the keys match, update the placeholder values
            replacements = jsonReplacements;
            updatePlaceholderValuesFromReplacements(currentPageIndex);
            QMessageBox::information(this, tr("Fill from JSON"), tr("Placeholders populated from JSON file."));
        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::onChooseDocPathClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Document"), QDir::homePath(), tr("Word Documents (*.docx)"));
    if (!filePath.isEmpty()) {
        try {
            docPath = filePath.toStdString();
            replacements = findPlaceholdersInDocument(docPath);
            createDynamicPlaceholders(replacements);
            QMessageBox::information(this, tr("Document Loaded"), tr("Placeholders loaded from the selected document."));


            std::ofstream file("paths.txt", std::ios::app);

            if (file.is_open()) {

                file << docPath;

                file.close();

            }


            std::filesystem::path name = filePath.toStdString();

            upload_template(name);

            ui->listWidget->clear();

            if (!template_list().empty()) {

                for (int i = 0; i < template_list().size(); i++) {

                    QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(template_list()[i]));
                    ui->listWidget->addItem(item);

                }

            }



        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), QString::fromStdString(e.what()));
        }
    }

}

void MainWindow::onSaveDraftClicked() {
    updateReplacementsFromInputs(pageSpinBox->value());
    QString selectedFilter;
    QString filePath = QFileDialog::getSaveFileName(this, "Save JSON File", QDir::homePath(), "JSON Files (*.json)", &selectedFilter);

    if (!filePath.isEmpty()) {
        try {
            saveJsonToFile(replacements, filePath.toStdString());
            QMessageBox::information(this, "Success", "Placeholder values saved to " + filePath);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::on_selectFileToView_clicked()
{

    QString path = ui->viewPathsListWidget->currentItem()->text();

    std::string commandString = "/opt/homebrew/bin/pandoc -f docx -t plain " + path.toStdString() + " -o output.txt";

    const char * command = commandString.c_str();

    int status = system(command);

    qDebug() << status;

    if (status != 0) {

        perror("Bad status");
        exit(1);

    }

    QFile file("output.txt");

    if (!file.open(QIODevice::ReadOnly)) {

        QMessageBox::information(0, "info", file.errorString());

    }

    QTextStream in (&file);

    ui->textBrowser->setText(in.readAll());

    ui->viewPathsListWidget->setVisible(false);
    ui->textBrowser->setVisible(true);


}


void MainWindow::on_fontButton_clicked()
{

    fontSize = ui->fontSelector->currentText().toStdString();

}


void MainWindow::on_ColorButton_clicked()
{
    color = ui->ColorSelector->currentText().toStdString();

}

