#include <QTest>
#include <QFile>

class Tests : public QObject
{
    Q_OBJECT

private slots:
void initTestCase() {
    // Create a directory for testing
    
}
void test_uploadTemplate_validFile() {
    fs::path filepath = "path/to/valid_template.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == 0, "upload_template failed for a valid file");
    // Additional assertions to check if the file and folder were created correctly
    QVERIFY2(fs::exists("path/to/import_dir/valid_template"), "Folder for template was not created");
    QVERIFY2(fs::exists("path/to/import_dir/valid_template/valid_template.docx"), "File was not copied to the correct location");
}

void test_uploadTemplate_invalidExtension() {
    fs::path filepath = "path/to/invalid_template.txt";
    int result = upload_template(filepath);
    QVERIFY2(result == -1, "upload_template failed for a file with invalid extension");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(!fs::exists("path/to/import_dir/invalid_template"), "Folder for template was created");
    QVERIFY2(!fs::exists("path/to/import_dir/invalid_template/invalid_template.txt"), "File was copied to an incorrect location");
}

void test_uploadTemplate_existingFolder() {
    fs::path filepath = "path/to/existing_template.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == -5, "upload_template failed for a file with an existing folder");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(fs::exists("path/to/import_dir/existing_template"), "Folder for template was not created");
    QVERIFY2(!fs::exists("path/to/import_dir/existing_template/existing_template.docx"), "File was copied to an existing folder");
}

void test_uploadTemplate_createFolderError() {
    fs::path filepath = "path/to/error_template.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == -4, "upload_template failed to create a folder for the template");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(!fs::exists("path/to/import_dir/error_template"), "Folder for template was created");
    QVERIFY2(!fs::exists("path/to/import_dir/error_template/error_template.docx"), "File was copied to an incorrect location");
}

void test_uploadTemplate_copyFileError() {
    fs::path filepath = "path/to/error_file.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == -6, "upload_template failed to copy the file");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(!fs::exists("path/to/import_dir/error_file"), "Folder for template was created");
    QVERIFY2(!fs::exists("path/to/import_dir/error_file/error_file.docx"), "File was copied to an incorrect location");
}

void test_deleteTemplate_validFile() {
    std::string filename = "valid_template.docx";
    int result = delete_template(filename);
    QVERIFY2(result == 0, "delete_template failed for a valid file");
    // Additional assertions to check if the folder and its contents were deleted
    QVERIFY2(!fs::exists("path/to/import_dir/valid_template"), "Folder for template was not deleted");
}

void test_deleteTemplate_nonExistingFolder() {
    std::string filename = "non_existing_template.docx";
    int result = delete_template(filename);
    QVERIFY2(result == -1, "delete_template failed for a non-existing folder");
    // Additional assertions to check if the folder and its contents were not deleted
    QVERIFY2(!fs::exists("path/to/import_dir/non_existing_template"), "Folder for template was deleted");
}

void test_deleteTemplate_deleteError() {
    std::string filename = "delete_error_template.docx";
    int result = delete_template(filename);
    QVERIFY2(result == -2, "delete_template failed to delete the folder");
    // Additional assertions to check if the folder and its contents were not deleted
    QVERIFY2(fs::exists("path/to/import_dir/delete_error_template"), "Folder for template was deleted");
}

void test_exportDocument_validFile() {
    std::string filename = "valid_document.txt";
    int result = export_document(filename);
    QVERIFY2(result == 0, "export_document failed for a valid file");
    // Additional assertions to check if the file was created correctly
    QVERIFY2(fs::exists(EXPORT_DIR / filename), "File was not saved in the correct location");
}

void test_exportDocument_fileNotFound() {
    std::string filename = "nonexistent_document.txt";
    int result = export_document(filename);
    QVERIFY2(result == -1, "export_document failed for a nonexistent file");
    // Additional assertions to check if the file was not created
    QVERIFY2(!fs::exists(EXPORT_DIR / filename), "File was saved even though it does not exist");
}

void test_exportDocument_createDirectoryError() {
    std::string filename = "document.txt";
    int result = export_document(filename);
    QVERIFY2(result == -2, "export_document failed to create the directory for saving the file");
    // Additional assertions to check if the file was not created
    QVERIFY2(!fs::exists(EXPORT_DIR / filename), "File was saved even though the directory could not be created");
}

void test_exportDocument_fileOpenError() {
    std::string filename = "invalid_document.txt";
    int result = export_document(filename);
    QVERIFY2(result == -3, "export_document failed to open the file for writing");
    // Additional assertions to check if the file was not created
    QVERIFY2(!fs::exists(EXPORT_DIR / filename), "File was saved even though the file could not be opened");
}

void test_templateList_emptyDirectory() {
    // Create an empty directory for testing
    fs::create_directory(IMPORT_DIR);

    std::vector<std::string> result = template_list();

    QVERIFY2(result.empty(), "template_list failed to return an empty vector for an empty directory");

    // Clean up the test directory
    fs::remove(IMPORT_DIR);
}

void test_templateList_nonEmptyDirectory() {
    // Create a directory with some templates for testing
    fs::create_directory(IMPORT_DIR);
    fs::create_directory(IMPORT_DIR + "/template1");
    fs::create_directory(IMPORT_DIR + "/template2");
    fs::create_directory(IMPORT_DIR + "/template3");

    std::vector<std::string> result = template_list();

    QVERIFY2(result.size() == 3, "template_list failed to return the correct number of templates");

    // Check if the returned templates match the expected ones
    QVERIFY2(std::find(result.begin(), result.end(), "template1") != result.end(), "template_list failed to return template1");
    QVERIFY2(std::find(result.begin(), result.end(), "template2") != result.end(), "template_list failed to return template2");
    QVERIFY2(std::find(result.begin(), result.end(), "template3") != result.end(), "template_list failed to return template3");

    // Clean up the test directory
    fs::remove_all(IMPORT_DIR);
}
void test_searchTemplateList_emptyList() {
    std::vector<std::string> result = search_template_list("key");
    QVERIFY2(result.empty(), "search_template_list failed for an empty list");
}

void test_searchTemplateList_noMatchingStrings() {
    std::vector<std::string> result = search_template_list("key");
    QVERIFY2(result.empty(), "search_template_list failed for no matching strings");
}

void test_searchTemplateList_matchingStrings() {
    std::vector<std::string> temp_list = {"key1", "key2", "key3"};
    std::vector<std::string> result = search_template_list("key");
    QVERIFY2(result.size() == 3, "search_template_list failed for matching strings");
    QVERIFY2(result[0] == "key1", "search_template_list failed for matching strings");
    QVERIFY2(result[1] == "key2", "search_template_list failed for matching strings");
    QVERIFY2(result[2] == "key3", "search_template_list failed for matching strings");
}
    void test_MainWindowVisibility() {
        // Create a new instance of the main window
        MainWindow w;
        w.show();
        // Check if the window is visible
        //
        QVERIFY2(w.isVisible(), "Window is not visible");
    }
    //replicate test for all buttons (Ask Aneesh)
    void test_WindowButton() {
        // Create a new instance of the main window
        MainWindow w;
        w.show();
        // Find the button
        QPushButton* button = w.findChild<QPushButton*>("button");
        QVERIFY2(button != nullptr, "Could not find the button");
        // Check if the button is enabled
        QVERIFY2(button->isEnabled(), "Button is not enabled");
    }
    void test_openTemplate() {
        // Create a new instance of the main window
        MainWindow w;
        w.show(); //replace with open a template through the gui
        //check if opening a template displays the correct text boxes
        // Find the textbox
        QList<QLineEdit*> lineEdits = window.findChildren<QLineEdit*>();

        for(QLineEdit* lineEdit : lineEdits) {
            QVERIFY2(lineEdit->text().isEmpty(), "Found a text box that is not empty");
            QVERIFY2(lineEdit->text().isEnabled(), "Found a text box that is not enabled");
        }
        
    }
    //dont know the actual process of parsing, opening, reading files.
    void test_replacePlaceholders() { //processing module
        QString documentContent = "<Placeholder1> <Placeholder2> <Placeholder3>";
        QMap<QString, QString> placeholderValues;
        placeholderValues["<Placeholder1>"] = "Value1";
        placeholderValues["<Placeholder2>"] = "Value2";
        placeholderValues["<Placeholder3>"] = "Value3";
        QString expectedFinalText = "Value1 Value2 Value3";
        QString finalText = replacePlaceholders(documentContent, placeholderValues);
        QVERIFY2(compareString(finalText, expectedFinalText), "Failed replacing placeholders");
    }
    void test_parsing() {
        

    } 
    void test_inputModule() { //test different custom profiles or placeholders

    }
    void test_save_document() {
        open(File)
        QString testData = "<Placeholder>"; //replace with what to save
        saveFile(File, testData);
        QVERIFY2(checkFileContent(File, testData), "File does not saved correctly");

    }
    void test_main() { //WIP
        
        open(File);
        fileContent = <possible read function>
        list = parse(fileContent);
        //test parsing
        QVERIFY2(checkList(list, expectedList), "Not parsed correctly");

        //test replacing placeholders
        QString finalText = replacePlaceholders(documentContent, placeholderValues);

        QVERIFY2(compareString(finalText, expectedFinalText), failed replacing placeholders);

        //test generatingDocument
    
        QString testData = "Test content to be saved into file.";
        saveFile(File, testData);
        QVERIFY2(checkFileContent("testFile.txt", testData), "File content does not match expected data.");
        
    }

private:


    // Function to check if file content matches expected data
    bool checkFileContent(const QString& fileName, const QString& expectedData)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        return fileContent == expectedData;
    }
};

QTEST_APPLESS_MAIN(MyTest)
#include <QTest>
#include <QFile>

class Tests : public QObject
{
    Q_OBJECT

private slots:
void initTestCase() {
    // Create a directory for testing
    
}
void test_uploadTemplate_validFile() {
    fs::path filepath = "path/to/valid_template.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == 0, "upload_template failed for a valid file");
    // Additional assertions to check if the file and folder were created correctly
    QVERIFY2(fs::exists("path/to/import_dir/valid_template"), "Folder for template was not created");
    QVERIFY2(fs::exists("path/to/import_dir/valid_template/valid_template.docx"), "File was not copied to the correct location");
}

void test_uploadTemplate_invalidExtension() {
    fs::path filepath = "path/to/invalid_template.txt";
    int result = upload_template(filepath);
    QVERIFY2(result == -1, "upload_template failed for a file with invalid extension");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(!fs::exists("path/to/import_dir/invalid_template"), "Folder for template was created");
    QVERIFY2(!fs::exists("path/to/import_dir/invalid_template/invalid_template.txt"), "File was copied to an incorrect location");
}

void test_uploadTemplate_existingFolder() {
    fs::path filepath = "path/to/existing_template.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == -5, "upload_template failed for a file with an existing folder");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(fs::exists("path/to/import_dir/existing_template"), "Folder for template was not created");
    QVERIFY2(!fs::exists("path/to/import_dir/existing_template/existing_template.docx"), "File was copied to an existing folder");
}

void test_uploadTemplate_createFolderError() {
    fs::path filepath = "path/to/error_template.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == -4, "upload_template failed to create a folder for the template");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(!fs::exists("path/to/import_dir/error_template"), "Folder for template was created");
    QVERIFY2(!fs::exists("path/to/import_dir/error_template/error_template.docx"), "File was copied to an incorrect location");
}

void test_uploadTemplate_copyFileError() {
    fs::path filepath = "path/to/error_file.docx";
    int result = upload_template(filepath);
    QVERIFY2(result == -6, "upload_template failed to copy the file");
    // Additional assertions to check if the file and folder were not created
    QVERIFY2(!fs::exists("path/to/import_dir/error_file"), "Folder for template was created");
    QVERIFY2(!fs::exists("path/to/import_dir/error_file/error_file.docx"), "File was copied to an incorrect location");
}

void test_deleteTemplate_validFile() {
    std::string filename = "valid_template.docx";
    int result = delete_template(filename);
    QVERIFY2(result == 0, "delete_template failed for a valid file");
    // Additional assertions to check if the folder and its contents were deleted
    QVERIFY2(!fs::exists("path/to/import_dir/valid_template"), "Folder for template was not deleted");
}

void test_deleteTemplate_nonExistingFolder() {
    std::string filename = "non_existing_template.docx";
    int result = delete_template(filename);
    QVERIFY2(result == -1, "delete_template failed for a non-existing folder");
    // Additional assertions to check if the folder and its contents were not deleted
    QVERIFY2(!fs::exists("path/to/import_dir/non_existing_template"), "Folder for template was deleted");
}

void test_deleteTemplate_deleteError() {
    std::string filename = "delete_error_template.docx";
    int result = delete_template(filename);
    QVERIFY2(result == -2, "delete_template failed to delete the folder");
    // Additional assertions to check if the folder and its contents were not deleted
    QVERIFY2(fs::exists("path/to/import_dir/delete_error_template"), "Folder for template was deleted");
}

void test_exportDocument_validFile() {
    std::string filename = "valid_document.txt";
    int result = export_document(filename);
    QVERIFY2(result == 0, "export_document failed for a valid file");
    // Additional assertions to check if the file was created correctly
    QVERIFY2(fs::exists(EXPORT_DIR / filename), "File was not saved in the correct location");
}

void test_exportDocument_fileNotFound() {
    std::string filename = "nonexistent_document.txt";
    int result = export_document(filename);
    QVERIFY2(result == -1, "export_document failed for a nonexistent file");
    // Additional assertions to check if the file was not created
    QVERIFY2(!fs::exists(EXPORT_DIR / filename), "File was saved even though it does not exist");
}

void test_exportDocument_createDirectoryError() {
    std::string filename = "document.txt";
    int result = export_document(filename);
    QVERIFY2(result == -2, "export_document failed to create the directory for saving the file");
    // Additional assertions to check if the file was not created
    QVERIFY2(!fs::exists(EXPORT_DIR / filename), "File was saved even though the directory could not be created");
}

void test_exportDocument_fileOpenError() {
    std::string filename = "invalid_document.txt";
    int result = export_document(filename);
    QVERIFY2(result == -3, "export_document failed to open the file for writing");
    // Additional assertions to check if the file was not created
    QVERIFY2(!fs::exists(EXPORT_DIR / filename), "File was saved even though the file could not be opened");
}

void test_templateList_emptyDirectory() {
    // Create an empty directory for testing
    fs::create_directory(IMPORT_DIR);

    std::vector<std::string> result = template_list();

    QVERIFY2(result.empty(), "template_list failed to return an empty vector for an empty directory");

    // Clean up the test directory
    fs::remove(IMPORT_DIR);
}

void test_templateList_nonEmptyDirectory() {
    // Create a directory with some templates for testing
    fs::create_directory(IMPORT_DIR);
    fs::create_directory(IMPORT_DIR + "/template1");
    fs::create_directory(IMPORT_DIR + "/template2");
    fs::create_directory(IMPORT_DIR + "/template3");

    std::vector<std::string> result = template_list();

    QVERIFY2(result.size() == 3, "template_list failed to return the correct number of templates");

    // Check if the returned templates match the expected ones
    QVERIFY2(std::find(result.begin(), result.end(), "template1") != result.end(), "template_list failed to return template1");
    QVERIFY2(std::find(result.begin(), result.end(), "template2") != result.end(), "template_list failed to return template2");
    QVERIFY2(std::find(result.begin(), result.end(), "template3") != result.end(), "template_list failed to return template3");

    // Clean up the test directory
    fs::remove_all(IMPORT_DIR);
}
void test_searchTemplateList_emptyList() {
    std::vector<std::string> result = search_template_list("key");
    QVERIFY2(result.empty(), "search_template_list failed for an empty list");
}

void test_searchTemplateList_noMatchingStrings() {
    std::vector<std::string> result = search_template_list("key");
    QVERIFY2(result.empty(), "search_template_list failed for no matching strings");
}

void test_searchTemplateList_matchingStrings() {
    std::vector<std::string> temp_list = {"key1", "key2", "key3"};
    std::vector<std::string> result = search_template_list("key");
    QVERIFY2(result.size() == 3, "search_template_list failed for matching strings");
    QVERIFY2(result[0] == "key1", "search_template_list failed for matching strings");
    QVERIFY2(result[1] == "key2", "search_template_list failed for matching strings");
    QVERIFY2(result[2] == "key3", "search_template_list failed for matching strings");
}
    void test_MainWindowVisibility() {
        // Create a new instance of the main window
        MainWindow w;
        w.show();
        // Check if the window is visible
        //
        QVERIFY2(w.isVisible(), "Window is not visible");
    }
    //replicate test for all buttons (Ask Aneesh)
    void test_WindowButton() {
        // Create a new instance of the main window
        MainWindow w;
        w.show();
        // Find the button
        QPushButton* button = w.findChild<QPushButton*>("button");
        QVERIFY2(button != nullptr, "Could not find the button");
        // Check if the button is enabled
        QVERIFY2(button->isEnabled(), "Button is not enabled");
    }
    void test_openTemplate() {
        // Create a new instance of the main window
        MainWindow w;
        w.show(); //replace with open a template through the gui
        //check if opening a template displays the correct text boxes
        // Find the textbox
        QList<QLineEdit*> lineEdits = window.findChildren<QLineEdit*>();

        for(QLineEdit* lineEdit : lineEdits) {
            QVERIFY2(lineEdit->text().isEmpty(), "Found a text box that is not empty");
            QVERIFY2(lineEdit->text().isEnabled(), "Found a text box that is not enabled");
        }
        
    }
    //dont know the actual process of parsing, opening, reading files.
    void test_replacePlaceholders() { //processing module
        QString documentContent = "<Placeholder1> <Placeholder2> <Placeholder3>";
        QMap<QString, QString> placeholderValues;
        placeholderValues["<Placeholder1>"] = "Value1";
        placeholderValues["<Placeholder2>"] = "Value2";
        placeholderValues["<Placeholder3>"] = "Value3";
        QString expectedFinalText = "Value1 Value2 Value3";
        QString finalText = replacePlaceholders(documentContent, placeholderValues);
        QVERIFY2(compareString(finalText, expectedFinalText), "Failed replacing placeholders");
    }
    void test_parsing() {
        

    } 
    void test_inputModule() { //test different custom profiles or placeholders

    }
    void test_save_document() {
        open(File)
        QString testData = "<Placeholder>"; //replace with what to save
        saveFile(File, testData);
        QVERIFY2(checkFileContent(File, testData), "File does not saved correctly");

    }
    void test_main() { //WIP
        
        open(File);
        fileContent = <possible read function>
        list = parse(fileContent);
        //test parsing
        QVERIFY2(checkList(list, expectedList), "Not parsed correctly");

        //test replacing placeholders
        QString finalText = replacePlaceholders(documentContent, placeholderValues);

        QVERIFY2(compareString(finalText, expectedFinalText), failed replacing placeholders);

        //test generatingDocument
    
        QString testData = "Test content to be saved into file.";
        saveFile(File, testData);
        QVERIFY2(checkFileContent("testFile.txt", testData), "File content does not match expected data.");
        
    }

private:


    // Function to check if file content matches expected data
    bool checkFileContent(const QString& fileName, const QString& expectedData)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        return fileContent == expectedData;
    }
};

QTEST_APPLESS_MAIN(MyTest)
