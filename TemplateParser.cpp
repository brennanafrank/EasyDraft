//
// Created by Brennan Frank on 2/18/24.
//

#ifndef TEMPLATEPARSER_CPP
#define TEMPLATEPARSER_CPP
#include "KZip/KZip.hpp"

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;
using recursive_directory_iterator = filesystem::recursive_directory_iterator;

void parse(string fullFilePath) {
    fs::path tempdocxparsing("./tempdocxparsing");
    cout << "here1.6 \n";
    fs::create_directory(tempdocxparsing);
    cout << "here1 \n";

    const fs::path docxpath("stat511.docx");

    /*
    // rb is mode read-binary, open a binary file for reading
    FILE* inputDocx = fopen(fullFilePath.c_str(), "rb");
    if (!inputDocx) {
        cerr << "There was an error opening the docx input file" << "\n";
        return;
    }

    // TODO fix later
    string outputpath = "";
    // wb, write a binary file
    FILE* output = fopen(outputpath.c_str(), "wb");
    if (!output) {
        cerr << "There was an error opening the docx output file" << "\n";
        // close original file descriptor
        fclose(inputDocx);
        return;
    }
     */

    KZip::ZipArchive input; // load zipFileName
    input.create(docxpath);
    cout << "here2\n";
    for (string s : input.entryNames()) {
        cout << s << "\n";
    }
    cout << "here3\n";

    mz_zip_archive zip_archive;




    /* later */
    for (const auto& dirEntry : recursive_directory_iterator("./tempdocxparsing")) {
        cout << dirEntry << endl;
    }

    // remove the temporary directory we created
    system("rm -rf ./tempdocxparsing"); // #TODO make system independent
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
