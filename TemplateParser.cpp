//
// Created by Brennan Frank on 2/18/24.
//

#ifndef TEMPLATEPARSER_CPP
#define TEMPLATEPARSER_CPP

#include "TemplateParser.hpp"

void parse(const string fullFilePath) {
    cout << "here1.6 \n";
    fs::create_directory(fs::path("./tempdocxparsing"));
    cout << "here1 \n";

    // replaces all occurances, so we might have to be careful if there's bugs with this
    std::string zipString = "./tempdocxparsing/" + std::regex_replace(fullFilePath, std::regex(".docx"), ".zip");
    fs::copy(fs::path(fullFilePath), fs::path(zipString), fs::copy_options::overwrite_existing);
    //elz::extractZip(zipString);
    cout << "here2\n";
    /*vector<string> entryNames = input.entryNames();
    cout << entryNames.size() << "\n";
    vector<KZip::ZipEntryProxy> xmlFiles{};
    for (int i = 0; i < entryNames.size(); i++) {
        int nameLen = entryNames[i].size();
        if (entryNames[i].find(".xml") == nameLen - 4) {

            KZip::ZipEntryProxy currentXml = input.entry(fs::path(entryNames[i]));
            // get data as a string
            std::string data = currentXml.getData<std::string>();
            // uses the ignore case flag
            //std::string replacedData = std::regex_replace(data, std::regex("(\\s*)\\$\\$COOL NOUN\\$\\$(\\s*)", std::regex::icase),
             //                                             "$1business$2");
            std::string replacedData = std::regex_replace(data, std::regex("COOL NOUN"),
                                                         "business");
            currentXml.setData<std::string>(replacedData);
            assert(replacedData == currentXml.getData<std::string>());
            if (entryNames[i] == "word/document.xml") {
                cout << replacedData << "\n";
            }
        }
    }

    cout << zipString << "\n";
    input.save();
    input.close();
    cout << "here3\n";


     */
    /* later */
    for (const auto& dirEntry : recursive_directory_iterator("./tempdocxparsing")) {
        cout << dirEntry << endl;
    }
}

class TemplateParser {
public:

private:
    // path to the docx file
    string docxPath = "";
    vector<string> replacements;


    // set the replacements, like from the input module, etc

    void setReplacements(const vector<string>& replacements) {
        this->replacements = replacements;
    }


};









#endif // TEMPLATEPARSER_CPP
