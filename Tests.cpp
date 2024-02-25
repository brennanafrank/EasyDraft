#include <QTest>
#include <QFile>

class Tests : public QObject
{
    Q_OBJECT

private slots:
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
