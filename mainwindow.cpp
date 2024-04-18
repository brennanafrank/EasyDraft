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


#include <QDebug>



// Q entry list
// Q list widget
//  Upload the file and return the file



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tagManager(new TagManager(TAG_MANAGER_JSON_PATH))
{
    ui->setupUi(this);
    loadSettings();
    ui->stackedWidget->setCurrentIndex(0);
    currentPageIndex = 1;


    updateTagComboBox();
    connect(ui->tagComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTagSelected(int)));

    QPixmap pix(":/rec/340.png");

    ui->back_button_export_view->setIcon(QIcon(pix));
    ui->back_button_export_view->setIconSize(pix.size());

    ui->back_button_parsing->setIcon(QIcon(pix));
    ui->back_button_parsing->setIconSize(pix.size());

    ui->back_button_view_doc->setIcon(QIcon(pix));
    ui->back_button_view_doc->setIconSize(pix.size());



    fileSystemModel = new CustomFileSystemModel(this);
    fileSystemModel->setRootPath(TEMPLATES_PATH);
    fileSystemModel->setTagManager(tagManager);

    QModelIndex templateIndex = fileSystemModel->index(TEMPLATES_PATH);

    ui->pathViewer->setModel(fileSystemModel);
    ui->pathViewer->setRootIndex(templateIndex);
    ui->pathViewer->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->pathViewer->setColumnWidth(0, 200); // set width of Name
    ui->pathViewer->setColumnWidth(3, 120); // Set width of date modified

    // Enable sorting and set initial sort order
    ui->pathViewer->setSortingEnabled(true);
    ui->pathViewer->sortByColumn(0, Qt::AscendingOrder); // Sort by the first column in ascending order

    // Enable drag-drop feature
    ui->pathViewer->setDragEnabled(true);
    ui->pathViewer->setAcceptDrops(true);
    ui->pathViewer->setDropIndicatorShown(true);
    ui->pathViewer->setDragDropMode(QAbstractItemView::DragDrop);
    ui->pathViewer->setDragDropOverwriteMode(false);
    ui->pathViewer->setDefaultDropAction(Qt::MoveAction);


    expandAllNodes(templateIndex);

    connect(ui->pathViewer, &QWidget::customContextMenuRequested, this, &MainWindow::showContextMenuForPathViewer);
    connect(ui->fillFromJsonButton, &QPushButton::clicked, this, [this](){
        onFillFromJsonClicked("");  // Calls the function without any parameters
    });
    ui->charMaxLimitSpinBox->setValue(maxCharLimit);


    //Auto-save initialization
    autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSaveDraft);
    autoSaveTimer->setInterval(1500);  // Set the interval to 1.5 secs
    autoSaveTimer->setSingleShot(true);  // The timer will only fire once after each restart
    connect(ui->autoSaveToggle, &QCheckBox::stateChanged, this, &MainWindow::handleAutoSaveToggle);

    connect(ui->recentFilesList, &QListWidget::itemClicked, this, &MainWindow::onRecentFileClicked);

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
    ui->fileText->setFont(font);
    ui->recentText->setFont(font);
    ui->label->setFont(font);

    // Shortcut
    // Back to previous page
    new QShortcut(QKeySequence("Esc"), this, SLOT(on_actionBack_triggered()));

    // Shortcut for forward page navigation (Cmd + ])
    new QShortcut(QKeySequence("Ctrl+]"), this, [this]() {
        int currentValue = pageSpinBox->value();
        int maxValue = pageSpinBox->maximum();
        if (currentValue < maxValue) {
            pageSpinBox->setValue(currentValue + 1);
        }
    });

    // Shortcut for backward page navigation (Cmd + [)
    new QShortcut(QKeySequence("Ctrl+["), this, [this]() {
        int currentValue = pageSpinBox->value();
        int minValue = pageSpinBox->minimum();
        if (currentValue > minValue) {
            pageSpinBox->setValue(currentValue - 1);
        }
    });

    // Context Menu Activation with adjustment for keyboard shortcut
    new QShortcut(QKeySequence("Ctrl+Shift+M"), this, [this]() {
        // Simulate a context menu event based on the current selection in pathViewer
        QModelIndex currentIndex = ui->pathViewer->currentIndex();
        if (currentIndex.isValid()) {
            // If there is a valid selection, show the context menu for that item
            QPoint simulatedPos = ui->pathViewer->visualRect(currentIndex).center();
            showContextMenuForPathViewer(simulatedPos);
        } else {
            // Fallback or handle cases with no selection
            showContextMenuForPathViewer(QPoint());
        }
    });

    // File Operations
    // Shortcut for deleting an item
    new QShortcut(QKeySequence("Ctrl+D"), this, [this]() {
        deleteItem();
    });

    // Shortcut for creating a folder
    new QShortcut(QKeySequence("Ctrl+N"), this, [this]() {
        QModelIndex currentIndex = ui->pathViewer->currentIndex();
        createFolder(currentIndex);
    });

    // Exporting Files
    new QShortcut(QKeySequence("Ctrl+E"), this, [this]() {
        onCompleteFillButtonlicked();
    });

    // Saving a Draft
    new QShortcut(QKeySequence("Ctrl+S"), this, [this]() {
        onSaveDraftClicked();
    });

    // Tag Management
    // Adding a Tag
    new QShortcut(QKeySequence("Ctrl+T"), this, [this]() {
        addTag();
    });

    // Deleting a Tag
    new QShortcut(QKeySequence("Ctrl+Shift+T"), this, [this]() {
        deleteTag();
    });

    // Toggle Auto-Save
    new QShortcut(QKeySequence("Ctrl+A"), this, [this]() {
        // Toggle the checkbox state
        bool checked = ui->autoSaveToggle->isChecked();
        ui->autoSaveToggle->setChecked(!checked);
        handleAutoSaveToggle(!checked ? Qt::Checked : Qt::Unchecked);
    });

    // Focus Recent Files widget
    new QShortcut(QKeySequence("Ctrl+Up"), this, [this]() {
        ui->recentFilesList->setFocus(); // Focuses the recent files list widget
    });

    // Focus on templates widget
    new QShortcut(QKeySequence("Ctrl+Down"), this, [this]() {
        ui->pathViewer->setFocus(); // Focuses the recent files list widget
    });

    // Shortcut for Fill From Json
    new QShortcut(QKeySequence("Ctrl+J"), this, [this]() {
        onFillFromJsonClicked("");
    });

    new QShortcut(QKeySequence("Ctrl+/"), this, [this]() {
        HelpDialog *helpDialog = new HelpDialog(this);
        helpDialog->setAttribute(Qt::WA_DeleteOnClose); // Ensure the dialog is deleted automatically after closing
        helpDialog->show();
    });


    // Install event filters
    ui->pathViewer->installEventFilter(this);
    ui->recentFilesList->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    //https://stackoverflow.com/questions/17032970/clear-data-inside-text-file-in-c
    // Clear the file
    std::ofstream ofs;
    ofs.open("savedpaths.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();

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

void MainWindow::on_pushButton_3_clicked()
{

    ui->stackedWidget->setCurrentWidget(ui->page_5);

}


void MainWindow::on_pushButton_5_clicked()
{


    if (viewPaths.empty()) {

        std::ifstream file("savedpaths.txt");

        if (file.is_open() && !dontAdd) {

            std::string line;

            while (std::getline(file, line)) {

                viewPaths.push_back(line);

            }

            file.close();

        }

    }

    QMessageBox::information(this, "Success", "Exported File" + QString::fromStdString(viewPaths[viewPaths.size() - 1]));

    std::string mainStringExport = viewPaths[viewPaths.size() - 1];

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Save Directory"), QDir::homePath());
    if (dirPath.isEmpty()) {
        return;
    }

    QMessageBox::information(this, "Success", "Exported File" + dirPath);


    std::string command = PYTHON_EXEC_PATH + " " + PROJECT_PATH + "/ExportDoc.py \"" + dirPath.toStdString() + "\" \"" + mainStringExport + "\" \"" + std::to_string(typeOfExport) + "\"";
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Python script failed with exit code " << result << std::endl;
    }

}



void MainWindow::on_PDFButton_clicked()
{

    MainWindow::typeOfExport = 1;

}

void MainWindow::on_TXTButton_clicked()
{

    MainWindow::typeOfExport = 2;

}


void MainWindow::on_HTMLButton_clicked()
{

    MainWindow::typeOfExport = 3;


}

void MainWindow::createFolder(const QModelIndex &index)
{
    // convert the model into FileSystemModel
    QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
    QString itemPath = model->filePath(index);
    QFileInfo fileInfo(itemPath);

    QString targetPath;

    if (index.isValid() && index.flags() & Qt::ItemIsSelectable) {
        // If a valid item is clicked, create a folder in that directory
        // Determine whether it is directory
            if (fileInfo.isDir()) {
                targetPath = itemPath;
            } else {
                targetPath = fileInfo.path();
            }
    } else {
        // If clicked on a blank area or an invalid item, create a folder in the template path
        targetPath = TEMPLATES_PATH;
    }

    // Get new file name from user
    QString folderName = QInputDialog::getText(this, tr("Create Folder"), tr("Folder Name:"));
    if (folderName.isEmpty()) return;

    // Construct the folderpath
    QString folderPath = QDir(targetPath).absoluteFilePath(folderName);

    // attempt to create folder
    if (!QDir().mkdir(folderPath)) {
        QMessageBox::warning(this, tr("Create Folder"), tr("Failed to create folder at %1.").arg(folderPath));
    }
}

void MainWindow::deleteAutoSaveAndTags(const QString &path) {
    QFileInfo fileInfo(path);
    if (fileInfo.isDir()) {
        QDir directory(path);
        QStringList allFiles = directory.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System, QDir::DirsFirst);
        for (const QString &file : allFiles) {
            QString fullPath = directory.absoluteFilePath(file);
            deleteAutoSaveAndTags(fullPath);  // Recursive call for files in directories
            recentFiles.removeAll(fullPath);  // Remove from the recent files list if present
        }
    } else {
        // Delete tags if any
        QStringList tags = tagManager->getTags(path);
        for (const QString &tag : tags) {
            tagManager->removeTag(path, tag);
        }

        // Generate the hashed filename based on the full path
        QString hashedName = hashFilePath(path);
        fs::path autoSaveFilePath = AUTO_SAVE_PATH / (hashedName.toStdString() + ".json");

        if (fs::exists(autoSaveFilePath)) {
            try {
                fs::remove(autoSaveFilePath);
            } catch (const std::filesystem::filesystem_error& e) {
                qDebug() << "Failed to delete auto-save file:" << QString::fromStdString(e.what());
            }
        }
    }
    recentFiles.removeAll(path);
    updateRecentFilesList();  // Refresh the UI list of recent files
}

void MainWindow::deleteItem() {
    QModelIndex index = ui->pathViewer->currentIndex();
    if (!index.isValid()) return;

    QFileSystemModel *model = static_cast<QFileSystemModel *>(ui->pathViewer->model());
    QModelIndex nameIndex = index.siblingAtColumn(0);
    QString itemPath = model->filePath(nameIndex);
    QString fileName = model->fileName(nameIndex);

    QString prompt = tr("Are you sure you want to delete '%1'?").arg(fileName);
    if (QMessageBox::question(this, tr("Delete Item"), prompt, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        deleteAutoSaveAndTags(itemPath);  // Use the recursive helper function

        if (!model->remove(nameIndex)) {
            QMessageBox::warning(this, tr("Delete Item"), tr("Failed to delete item."));
        } else {
            updateTagComboBox();
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


void MainWindow::addTag() {
    QString filePath = getCurrentSelectedFilePath();
    if (filePath.isEmpty()) {
        return;
    }

    int currentIndex = ui->tagComboBox->currentIndex();

    bool ok;
    QString tag = QInputDialog::getText(this, tr("Add Tag"),
                                        tr("Tag:"), QLineEdit::Normal,
                                        QDir::home().dirName(), &ok);
    if (ok && !tag.isEmpty()) {
        tagManager->addTag(filePath, tag);
        updateTagComboBox();

        if (currentIndex >= 0 && currentIndex < ui->tagComboBox->count()) {
            ui->tagComboBox->setCurrentIndex(currentIndex);
        } else {
            ui->tagComboBox->setCurrentIndex(currentIndex);
            ui->tagComboBox->setCurrentIndex(0);
        }
    }
    // qDebug() << ui->tagComboBox->currentIndex();
}

void MainWindow::deleteTag() {
    QString filePath = getCurrentSelectedFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a file first."));
        return;
    }

    int currentIndex = ui->tagComboBox->currentIndex();

    QStringList tags = tagManager->getTags(filePath);
    if (tags.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("Selected file has no tags to remove."));
        return;
    }

    bool ok;
    QString tagToDelete = QInputDialog::getItem(this, tr("Remove Tag"),
                                                tr("Select a tag to remove:"), tags, 0, false, &ok);
    if (ok && !tagToDelete.isEmpty()) {
        tagManager->removeTag(filePath, tagToDelete);
        updateTagComboBox();

        if (currentIndex >= 0 && currentIndex < ui->tagComboBox->count()) {
            ui->tagComboBox->setCurrentIndex(currentIndex);
        } else {
            ui->tagComboBox->setCurrentIndex(currentIndex);
            ui->tagComboBox->setCurrentIndex(0);
        }
    }
    // qDebug() << ui->tagComboBox->currentIndex();
}


void MainWindow::on_pushButton_6_clicked()
{


    std::ifstream file("savedpaths.txt");

    if (file.is_open() && !dontAdd) {

        std::string line;

        while (std::getline(file, line)) {

            viewPaths.push_back(line);

        }

        file.close();

    }


    // This is a work in progress code for reading a docx file

    ui->textBrowser->setVisible(false);

    if (!dontAdd) {

        for (int i = 0; i < viewPaths.size(); ++i) {

            QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(viewPaths[i]));

            ui->viewPathsListWidget->addItem(item);

        }

    }

    dontAdd = true;

    changePage();

}

void MainWindow::on_submitPlace_clicked()
{

   changePage();

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

    dontAdd = false;

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
            if (ui->autoSaveToggle->isChecked()) { // auto-save check
                autoSaveTimer->start();  // Restart the timer
            }
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
    updatePlaceholderValuesFromReplacements(currentPageIndex);
}



void MainWindow::onPageChanged(int newPage) {
    QFormLayout* layout = qobject_cast<QFormLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!layout) return;

    for (auto& pair : replacements) {
        pair.second.resize(std::max(pair.second.size(), static_cast<size_t>(newPage)));
    }
    updateReplacementsFromInputs(currentPageIndex);
    currentPageIndex = pageSpinBox->value();

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

void MainWindow::animateLineEditColorChange(QLineEdit* lineEdit, const QColor& startColor, const QColor& endColor, int duration) {
    if (!lineEdit) return; // Ensure the QLineEdit pointer is valid

    // Create an animation object for color transition
    QVariantAnimation *animation = new QVariantAnimation(lineEdit);
    animation->setDuration(duration); // Set the animation duration, default is 500 milliseconds

    // Set the start and end values of the animation to the provided colors
    animation->setStartValue(startColor);
    animation->setEndValue(endColor);

    // Update the QLineEdit background color during the animation
    connect(animation, &QVariantAnimation::valueChanged, [lineEdit](const QVariant &value) {
        QColor color = value.value<QColor>();
        lineEdit->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    });

    // After the animation is finished, create another animation to transition the color back to the start color
    connect(animation, &QVariantAnimation::finished, [lineEdit, startColor, endColor, duration]() {
        QVariantAnimation *returnAnimation = new QVariantAnimation(lineEdit);
        returnAnimation->setDuration(duration);
        returnAnimation->setStartValue(endColor);
        returnAnimation->setEndValue(startColor);

        connect(returnAnimation, &QVariantAnimation::valueChanged, [lineEdit](const QVariant &value) {
            QColor color = value.value<QColor>();
            lineEdit->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        });

        returnAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped); // Start the animation
}


// Update the char count label and check whether it reach the max char limit
void MainWindow::updateCharCountLabel(QLineEdit* lineEdit, QLabel* charCountLabel) {
    int currentLength = lineEdit->text().length();
    int maxLength = lineEdit->maxLength();
    charCountLabel->setText(QString("%1/%2").arg(currentLength).arg(maxLength));

    if (currentLength >= maxLength) {
        // Use the current base background color of the QLineEdit as the start color for the animation
        QColor startColor = lineEdit->palette().base().color();
        QColor endColor = QColor("#FFCCCC"); // Animation end color, light red
        int duration = 500; // Animation duration in milliseconds

        // Call the previously defined animation function
        animateLineEditColorChange(lineEdit, startColor, endColor, duration);
    }
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
    // qDebug() << "updateReplacementsFromInputs";
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
    removeEmptyValuesFromReplacements(replacements);

    bool allEmpty = true;
    for (const auto& pair : replacements) {
        if (!pair.second.empty()) {
            allEmpty = false;
            break;
        }
    }

    if (allEmpty) {
        QMessageBox::warning(this, "Empty Placeholders", "Please fill in at least one placeholder before proceeding.");
        return;
    }

    // Validate replacements before proceeding
    if (!isReplacementsValid(replacements)) {
        QMessageBox::warning(this, "Invalid Replacements", "One or more placeholders have empty values. Please fill in all placeholders before continuing.");

        // Check if any QLineEdit on the current page is empty
        QFormLayout* layout = qobject_cast<QFormLayout*>(ui->scrollAreaWidgetContents->layout());
        if (layout) {
            int currentPage = pageSpinBox->value();
            for (int i = 0; i < layout->rowCount() - 2; ++i) {
                QHBoxLayout* rowLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(i, QFormLayout::FieldRole)->layout());
                if (rowLayout && !rowLayout->isEmpty()) {
                    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(rowLayout->itemAt(0)->widget());
                    if (lineEdit && lineEdit->text().isEmpty()) {
                        QColor startColor = lineEdit->palette().base().color();
                        QColor endColor = QColor("#FFCCCC"); // Light red
                        animateLineEditColorChange(lineEdit, startColor, endColor, 1000);
                    }
                }
            }
        }

        return;
    }

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

    modifyDocument(docPath, vectorToJson(replacements), dirPath, prefix, color);

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



void MainWindow::onFillFromJsonClicked(const QString &filePath = QString()) {
    QString selectedFilePath = filePath;

    if (selectedFilePath.isEmpty()) {
        // If no file path is provided, show file dialog
        selectedFilePath = QFileDialog::getOpenFileName(this, tr("Open JSON File"), QDir::homePath(), tr("JSON Files (*.json)"));
    }
    else{ // find placeholder in docx first if prefill the placeholder needed
        replacements = findPlaceholdersInDocument(docPath);
    }

    if (!docPath.empty() && !selectedFilePath.isEmpty()) {
        try {
            std::vector<std::pair<std::string, std::vector<std::string>>> jsonReplacements = readJsonFromFile(selectedFilePath.toStdString());

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

            replacements = jsonReplacements;
            updatePlaceholderValuesFromReplacements(currentPageIndex);
            // QMessageBox::information(this, tr("Fill from JSON"), tr("Placeholders populated from JSON file."));
            // for (const auto& pair : replacements) {
            //     qDebug() << "Key:" << QString::fromStdString(pair.first);
            //     qDebug() << "Values:";
            //     for (const auto& value : pair.second) {
            //         qDebug() << QString::fromStdString(value);
            //     }
            // }
        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), QString::fromStdString(e.what()));
        }
    } else {
        // QMessageBox::warning(this, tr("Error"), tr("Please select a document first."));
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



void MainWindow::showContextMenuForPathViewer(const QPoint &pos)
{
    QModelIndex index = ui->pathViewer->indexAt(pos); // Check the clicking position

    QMenu contextMenu(tr("Context Menu"), this);
    QAction *newFolderAction = new QAction(tr("New Folder"), this);
    connect(newFolderAction, &QAction::triggered, this, [this, index]() {
        createFolder(index);
    });
    contextMenu.addAction(newFolderAction);

    QAction *addTagAction = new QAction(tr("Add Tag"), this);
    connect(addTagAction, &QAction::triggered, this, &MainWindow::addTag);
    contextMenu.addAction(addTagAction);


    QAction *deleteItemAction = new QAction(tr("Delete Item"), this);
    connect(deleteItemAction, &QAction::triggered, this, &MainWindow::deleteItem);
    contextMenu.addAction(deleteItemAction);

    QAction *deleteTagAction = new QAction(tr("Remove Tag"), this);
    connect(deleteTagAction, &QAction::triggered, this, &MainWindow::deleteTag);
    contextMenu.addAction(deleteTagAction);

    QPoint globalPos = ui->pathViewer->viewport()->mapToGlobal(pos);
    contextMenu.exec(globalPos);
}



void MainWindow::on_pathViewer_doubleClicked(const QModelIndex &index) {
    QString currentPath = fileSystemModel->filePath(index);
    docPath = currentPath.toStdString();

    if (!currentPath.endsWith(".docx", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, tr("Invalid File"), tr("The selected file is not a .docx document."));
        return;
    }
    updateRecentFiles(currentPath);  // Update the recent files list
    updateAutoSaveToggleState();
    QString hashedName = hashFilePath(QString::fromStdString(docPath));
    fs::path autoSaveFilePath = AUTO_SAVE_PATH / (hashedName.toStdString() + ".json");
    QFile autoSaveFile(QString::fromStdString(autoSaveFilePath.string()));
    if (autoSaveFile.exists()) {
        // Set the autoSaveToggle checked state to true
        ui->autoSaveToggle->setChecked(true);

        auto reply = QMessageBox::question(this, tr("Load Auto-Save"), tr("An auto-saved file exists. Would you like to load it?"), QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            onFillFromJsonClicked(QString::fromStdString(autoSaveFilePath.string()));  // Load the auto-saved data
        }
        else {
            replacements = findPlaceholdersInDocument(docPath); // Load placeholders from document without auto-saved data
        }
    }
    else {
        // Ensure that the autoSaveToggle is unchecked if no auto-save file exists
        ui->autoSaveToggle->setChecked(false);
        replacements = findPlaceholdersInDocument(docPath);
    }

    createDynamicPlaceholders(replacements, maxCharLimit);
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
    currentPageIndex = pageSpinBox->value();
    updatePlaceholderValuesFromReplacements(currentPageIndex);


    // qDebug()<< "on_pathViewer_doubleClicked";
    // for (const auto& pair : replacements) {
    //     qDebug() << "Key:" << QString::fromStdString(pair.first);
    //     qDebug() << "Values:";
    //     for (const auto& value : pair.second) {
    //         qDebug() << QString::fromStdString(value);
    //     }
    // }
}





void MainWindow::on_charMaxLimitButton_clicked()
{
    for (const auto& pair : replacements) {
        qDebug() << "Key:" << QString::fromStdString(pair.first);
        qDebug() << "Values:";
        for (const auto& value : pair.second) {
            qDebug() << QString::fromStdString(value);
        }
    }
    maxCharLimit = ui->charMaxLimitSpinBox->value();
    saveSettings();
}

void MainWindow::saveSettings() {
    QSettings settings("CS307_Gp37", "EasyDraft");
    settings.setValue("maxCharLimit", maxCharLimit);
    // Save the recent files
    settings.setValue("recentFiles", recentFiles.join("|"));
}


void MainWindow::loadSettings() {
    QSettings settings("CS307_Gp37", "EasyDraft");
    maxCharLimit = settings.value("maxCharLimit", 20).toInt();
    // Load the recent files
    QString files = settings.value("recentFiles", "").toString();
    if (!files.isEmpty()) {
        recentFiles = files.split("|");
    }

    // Update the UI list to reflect the loaded recent files
    updateRecentFilesList();
}

void MainWindow::onTagSelected(int index) {
    if (index >= 0) {
        QString tag = ui->tagComboBox->itemText(index);
        filterFilesByTag(tag);
    } else {
        filterFilesByTag("");
    }
}


void MainWindow::updateTagComboBox() {
    QStringList tags = tagManager->getAllTags();
    tags.insert(0, "All Tags");

    // Disconnect the signal before updating the combobox
    disconnect(ui->tagComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTagSelected(int)));

    ui->tagComboBox->clear();
    ui->tagComboBox->addItems(tags);

    // Reconnect the signal after updating the combobox
    connect(ui->tagComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTagSelected(int)));
}


void MainWindow::filterFilesByTag(const QString &tag) {
    // qDebug() << "Starting file filtering for tag:" << tag;

    QModelIndex rootIndex = fileSystemModel->index(TEMPLATES_PATH);
    expandAllNodes(rootIndex);

    filterIndexByTag(rootIndex, tag);
    // qDebug() << "Finished file filtering.";
}


void MainWindow::expandAllNodes(const QModelIndex &index) {
    // temporary disable update to hidden the expand process
    ui->pathViewer->setUpdatesEnabled(false);

    int rowCount = fileSystemModel->rowCount(index);
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex childIndex = fileSystemModel->index(i, 0, index);
        if (!childIndex.isValid()) {
            continue;
        }
        if (fileSystemModel->isDir(childIndex)) {
            ui->pathViewer->expand(childIndex);
            QApplication::processEvents();
            expandAllNodes(childIndex);
        }
    }
    ui->pathViewer->collapse(index);

    ui->pathViewer->setUpdatesEnabled(true);
}



bool MainWindow::filterIndexByTag(const QModelIndex &index, const QString &tag) {
    int rowCount = fileSystemModel->rowCount(index);
    bool anyVisible = false;

    for (int i = 0; i < rowCount; ++i) {
        QModelIndex childIndex = fileSystemModel->index(i, 0, index);
        if (!childIndex.isValid()) {
            continue;
        }

        QString filePath = fileSystemModel->filePath(childIndex);
        QStringList tags = tagManager->getTags(filePath);
        bool isVisible = tag == "All Tags" || tag.isEmpty() || tags.contains(tag, Qt::CaseInsensitive);

        if (fileSystemModel->isDir(childIndex)) {
            isVisible = filterIndexByTag(childIndex, tag) || isVisible;
        }

        ui->pathViewer->setRowHidden(i, index, !isVisible);
        anyVisible = anyVisible || isVisible;
    }

    return anyVisible;
}


void MainWindow::autoSaveDraft() {
    updateReplacementsFromInputs(currentPageIndex);
    if (!docPath.empty()) {
        QString hashedName = hashFilePath(QString::fromStdString(docPath));
        fs::path autoSaveFilePath = AUTO_SAVE_PATH / (hashedName.toStdString() + ".json");

        // Check if the directory exists and create it if it does not
        if (!fs::exists(autoSaveFilePath.parent_path())) {
            try {
                fs::create_directories(autoSaveFilePath.parent_path());
            } catch (const std::exception& e) {
                qDebug() << "Failed to create auto-save directory: " << QString::fromStdString(e.what());
                return;
            }
        }

        // Attempt to save the JSON file
        try {
            saveJsonToFile(replacements, autoSaveFilePath.string());
            // qDebug() << "Auto-saved successfully to:" << QString::fromStdString(autoSaveFilePath.string());
        } catch (const std::exception& e) {
            qDebug() << "Auto-save failed: " << QString::fromStdString(e.what());
        }
    }
}



void MainWindow::on_back_button_export_view_clicked()
{

    MainWindow::on_actionBack_triggered();

}


void MainWindow::on_back_button_parsing_clicked()
{

    MainWindow::on_actionBack_triggered();

}


void MainWindow::on_back_button_view_doc_clicked()
{

    MainWindow::on_actionBack_triggered();

}


void MainWindow::handleAutoSaveToggle(int state) {
    if (state == Qt::Unchecked && !docPath.empty()) {
        // Construct the path to the auto-save file
        QString hashedName = hashFilePath(QString::fromStdString(docPath));
        fs::path autoSaveFilePath = AUTO_SAVE_PATH / (hashedName.toStdString() + ".json");

        // Check if the auto-save file exists and delete it
        if (fs::exists(autoSaveFilePath)) {
            try {
                fs::remove(autoSaveFilePath);
                // qDebug() << "Auto-save file deleted successfully.";
            } catch (const std::filesystem::filesystem_error& e) {
                qDebug() << "Failed to delete auto-save file:" << QString::fromStdString(e.what());
                QMessageBox::warning(this, tr("Error Deleting Auto-Save File"),
                                     tr("Could not delete the auto-save file. Please check permissions and file status."));
            }
        }
    }
}

void MainWindow::updateAutoSaveToggleState() {
    if (docPath.empty()) {
        ui->autoSaveToggle->setChecked(false);
        return;
    }

    QString hashedName = hashFilePath(QString::fromStdString(docPath));
    fs::path autoSaveFilePath = AUTO_SAVE_PATH / (hashedName.toStdString() + ".json");
    bool exists = fs::exists(autoSaveFilePath);

    ui->autoSaveToggle->setChecked(exists);
}


QString MainWindow::hashFilePath(const QString& path) {
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(path.toUtf8());
    return hash.result().toHex();  // Converts the hash to a hexadecimal string
}

void MainWindow::updateRecentFiles(const QString& filePath) {
    recentFiles.removeAll(filePath);  // Remove if already exists to prevent duplicates
    recentFiles.prepend(filePath);    // Add to the front of the list
    if (recentFiles.size() > 5) {
        recentFiles.removeLast();     // Ensure only 5 items are kept
    }

    updateRecentFilesList();  // Update the UI
}



// void MainWindow::updateRecentFilesList() {
//     ui->recentFilesList->clear();
//     for (const QString& file : recentFiles) {
//         QListWidgetItem* item = new QListWidgetItem(QFileInfo(file).fileName(), ui->recentFilesList);
//         item->setData(Qt::UserRole, file);  // Store the file path as item data
//     }
// }


void MainWindow::updateRecentFilesList() {
    ui->recentFilesList->clear();
    for (const QString &filePath : recentFiles) {
        QListWidgetItem *item = new QListWidgetItem(ui->recentFilesList);
        QWidget *widget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;  // Horizontal layout

        QLabel *titleLabel = new QLabel(QFileInfo(filePath).fileName());
        QLabel *pathLabel = new QLabel(" - " + QFileInfo(filePath).absolutePath());
        pathLabel->setStyleSheet("color: grey;");

        layout->addWidget(titleLabel);
        layout->addWidget(pathLabel);
        layout->addStretch();  // Add a stretchable space at the end to push content to the left
        layout->setSpacing(2);  // Reduce space between labels if needed
        layout->setContentsMargins(5, 5, 5, 5);  // Set outer margins
        widget->setLayout(layout);

        item->setSizeHint(widget->sizeHint());
        ui->recentFilesList->addItem(item);
        ui->recentFilesList->setItemWidget(item, widget);

        // Set the file path data for retrieval when clicked
        item->setData(Qt::UserRole, filePath);
    }
    saveSettings();
}




void MainWindow::onRecentFileClicked(QListWidgetItem* item) {
    QString filePath = item->data(Qt::UserRole).toString();
    if (!filePath.isEmpty()) {
        // Simulate opening the file as if it was double-clicked in the path viewer
        QModelIndex index = fileSystemModel->index(filePath);
        on_pathViewer_doubleClicked(index);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            // Determine which widget is in focus and act accordingly
            if (obj == ui->pathViewer) {
                QModelIndex currentIndex = ui->pathViewer->currentIndex();
                if (currentIndex.isValid()) {
                    on_pathViewer_doubleClicked(currentIndex);
                    return true; // Event handled
                }
            } else if (obj == ui->recentFilesList) {
                QListWidgetItem *currentItem = ui->recentFilesList->currentItem();
                if (currentItem != nullptr) {
                    onRecentFileClicked(currentItem);
                    return true; // Event handled
                }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event); // Pass the event on to the parent class
}
