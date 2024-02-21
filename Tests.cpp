#include <QTest>
#include <QFile>

class Test : public QObject
{
    Q_OBJECT

private slots:
    void 
    void test_main() {
        
        open(File)
        fileContent = <possible read function>
        list = parse(fileContent)
        //test parsing
        QVERIFY2(checkList(list, expectedList), "Not parsed correctly")

        //test replacing placeholders
        QString finalText = replacePlaceholders(documentContent, placeholderValues);

        QVERIFY2(compareString(finalText, expectedFinalText), failed replacing placeholders);

        //test generatingDocument
    
        QString testData = "Test content to be saved into file.";
        saveFile("testFile.txt", testData);
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
