#ifndef EASYDRAFT_TEMPLATEPARSER_H
#define EASYDRAFT_TEMPLATEPARSER_H

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using recursive_directory_iterator = filesystem::recursive_directory_iterator;


class TemplateParser {
    // path to the docx file
private:
    string docxPath = nullptr;
    vector<string> replacements;


    // set the replacements, like from the input module, etc

    void setReplacements(const vector<string>& replacements);


};


#endif //EASYDRAFT_TEMPLATEPARSER_H
