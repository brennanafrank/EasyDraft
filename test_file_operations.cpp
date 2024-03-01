#include <QTest>
#include <QFile>
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