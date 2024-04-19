//
// Created by Brennan Frank on 2/18/24.
//

#ifndef TEMPLATEPARSER_CPP
#define TEMPLATEPARSER_CPP

<<<<<<< HEAD
#include <assert.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;
using recursive_directory_iterator = filesystem::recursive_directory_iterator;
#include "TemplateParser.hpp"

#ifndef PARSE_FUNC
#define PARSE_FUNC
void parse(const string fullFilePath) {
    cout << "here1.5 \n";
    std::system("rm -rf ./tempdocxparsing");
    fs::create_directory(fs::path("./tempdocxparsing"));
    cout << "here1 \n";

    // replaces all occurances, so we might have to be careful if there's bugs with this
    std::string zipString = "./tempdocxparsing/" + std::regex_replace(fullFilePath, std::regex(".docx"), ".zip");
    fs::copy(fs::path(fullFilePath), fs::path(zipString), fs::copy_options::overwrite_existing);

    cout << "unzip -o " + zipString + " -d ./tempdocxparsing\n";
    std::system(("unzip -o " + zipString + " -d ./tempdocxparsing").c_str());
    std::system(("rm " + zipString).c_str());




    // std::string discardedString;
    // std::string individualFilepath;
    for (const auto& dirEntry : recursive_directory_iterator("./tempdocxparsing")) {
        std::string path = dirEntry.path();
        // for some reason this comes with quotation marks
        path.erase(std::remove(path.begin(), path.end(), '"'), path.end());

        if (path.find(".xml") != path.size() - 4) {
            continue;
        }
        cout << path << "\n";

        std::ifstream inputFileStream(path);
        // originally .temp at the end
        std::ofstream outputFileStream(path + ".temp");
        std::string line;
        while (std::getline(inputFileStream, line)) {
            std::string replacedData = std::regex_replace(line, std::regex("(\\s*)\\$\\$COOL NOUN\\$\\$(\\s*)", std::regex::icase),
"$1business$2");
            outputFileStream << replacedData << "\n";
        }

        std::system(("cp " + path + ".temp " + path).c_str());
        std::system(("rm " + path + ".temp").c_str());

        outputFileStream.close();
        inputFileStream.close();
    }

    std::system(("zip -r " + fullFilePath + ".temp ./tempdocxparsing/").c_str());

}
#endif // PARSE_FUNC

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
