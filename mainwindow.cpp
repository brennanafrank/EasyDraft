#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customfilesystemmodel.h"





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tagManager(new TagManager("/Users/michael/Downloads/Test_Folder/config.json"))
    , placeholderManager(new PlaceholderManager(this))
{
    ui->setupUi(this);

    currentPageIndex = 1;
    placeholderManager->addDefaultPlaceholders();
    placeholderManager->loadPlaceholders();

    CustomFileSystemModel *fileSystemModel = new CustomFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());
    fileSystemModel->setTagManager(tagManager);
    ui->pathViewer->setModel(fileSystemModel);
    ui->pathViewer->setRootIndex(fileSystemModel->index("/Users/michael/Downloads/Test_Folder"));


    ui->pathViewer->setDragEnabled(true);
    ui->pathViewer->setAcceptDrops(true);
    ui->pathViewer->setDropIndicatorShown(true);

    connect(ui->createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(ui->deleteItemButton, &QPushButton::clicked, this, &MainWindow::deleteItem);
    connect(ui->deleteTagButton, &QPushButton::clicked, this, &MainWindow::deleteTag);
    connect(ui->addTagButton, &QPushButton::clicked, this, &MainWindow::onAddTagButtonClicked);


    connect(ui->addPlaceholderButton, &QPushButton::clicked, this, &MainWindow::onAddPlaceholderClicked);
    connect(ui->placeholderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlaceholderSelected(int)));
    connect(ui->deletePlaceholderButton, &QPushButton::clicked, this, &MainWindow::onDeletePlaceholderClicked);

    connect(ui->fillFromJsonButton, &QPushButton::clicked, this, &MainWindow::onFillFromJsonClicked);
    connect(ui->chooseDocPathButton, &QPushButton::clicked, this, &MainWindow::onChooseDocPathClicked);

    // Populate the combo box with existing placeholder names if any
    ui->placeholderComboBox->addItems(placeholderManager->getPlaceholderNames());
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

void MainWindow::onAddPlaceholderClicked() {
    bool ok;
    // Prompt user for placeholder name
    QString placeholderName = QInputDialog::getText(this, tr("Add Placeholder"),
                                                    tr("Placeholder Name:"), QLineEdit::Normal,
                                                    QString(), &ok);
    if (ok && !placeholderName.isEmpty()) {
        // Prompt user for placeholder value
        QString placeholderValue = QInputDialog::getText(this, tr("Placeholder Value"),
                                                         tr("Value for ") + placeholderName + ":", QLineEdit::Normal,
                                                         QString(), &ok);
        if (ok) {
            // Use PlaceholderManager to add placeholder
            placeholderManager->addPlaceholder(placeholderName, placeholderValue);
            placeholderManager->savePlaceholders();
            // Update UI ComboBox
            ui->placeholderComboBox->addItem(placeholderName);
        }
    }
}


void MainWindow::onPlaceholderSelected(int index) {
    if (index < 0) return; // Ensure valid index

    QString placeholderName = ui->placeholderComboBox->itemText(index);

    QWidget *focusedWidget = QApplication::focusWidget();

    // Check if the selected placeholder is the current date and time
    if (placeholderName == "CurrentDateTime") {
        // Dynamically generate the current date and time
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        ui->lineEdit->setText(currentDateTime);
    } else {
        // if (focusedWidget && focusedWidget->metaObject()->className() == QLatin1String("QLineEdit")) {
        //     QLineEdit *lineEdit = static_cast<QLineEdit *>(focusedWidget);
        //     QString value = placeholderManager->getPlaceholderValue(placeholderName);
        //     lineEdit->setText(value);
        // }
        // For other placeholders, fetch the stored value
        QString value = placeholderManager->getPlaceholderValue(placeholderName);
        ui->lineEdit->setText(value);
    }
}

void MainWindow::onDeletePlaceholderClicked() {

    QString placeholderName = ui->placeholderComboBox->currentText();
    if (!placeholderName.isEmpty()) {
        placeholderManager->removePlaceholder(placeholderName);
        ui->placeholderComboBox->removeItem(ui->placeholderComboBox->currentIndex());

        // Optionally, save placeholders to persist the deletion
        placeholderManager->savePlaceholders();
    }
}


void MainWindow::createDynamicPlaceholders(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacementsm, int maxCharLimit) {
    QWidget* contents = ui->scrollAreaWidgetContents;
    QFormLayout* layout = qobject_cast<QFormLayout*>(contents->layout());
    QPushButton* completeFillButton = new QPushButton("Complete", ui->scrollAreaWidgetContents);
    connect(completeFillButton, &QPushButton::clicked, this, &MainWindow::onCompleteFillButtonlicked);

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

    // ui->pageSpinBox->setMinimum(1);

    pageSpinBox = new QSpinBox(contents);
    pageSpinBox->setMinimum(1);
    connect(pageSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onPageChanged);

    QHBoxLayout* pageLayout = new QHBoxLayout();
    pageLayout->addWidget(pageLabel);

    pageLayout->addWidget(pageSpinBox);
    layout->addRow(pageLayout);

    layout->addRow(completeFillButton);
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
    saveJsonToFile(replacements, "/Users/michael/Developer/EasyDraft/testDoc.json");
    modifyDocument(docPath, vectorToJson(replacements));
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
            replacements = readJsonFromFile(filePath.toStdString());
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
        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), QString::fromStdString(e.what()));
        }
    }
}

MainWindow::~MainWindow()
{
    placeholderManager->savePlaceholders();
    delete ui;
    delete tagManager;
    delete placeholderManager;
}

