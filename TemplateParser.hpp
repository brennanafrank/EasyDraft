#ifndef EASYDRAFT_TEMPLATEPARSER_HPP
#define EASYDRAFT_TEMPLATEPARSER_HPP
#include <zip_file.hpp>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;
using recursive_directory_iterator = filesystem::recursive_directory_iterator;


class TemplateParser {
public:
    TemplateParser();
    void parse(string fullFilePath);
private:
    // path to the docx file
    string docxPath = "";
    vector<string> replacements;


    // set the replacements, like from the input module, etc

    void setReplacements(const vector<string>& replacements);


};


#endif //EASYDRAFT_TEMPLATEPARSER_HPP
