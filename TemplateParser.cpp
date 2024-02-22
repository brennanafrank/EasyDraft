//
// Created by Brennan Frank on 2/18/24.
//

#include <filesystem>
#include "miniz-cpp/zip_file.hpp" // miniz tutorial: https://www.informit.com/articles/article.aspx?p=3150354&seqNum=13
#include "TemplateParser.h"

using namespace std;
namespace fs = filesystem;

void TemplateParser::setReplacements(const vector<string>& replacements) {
    this->replacements = replacements;
}

// the file path has to be in full
void parse(string fullFilePath) {
    fs::path tempdocxparsing("./tempdocxparsing");
    fs::create_directory(tempdocxparsing);

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

    mz_zip_archive zip_archive;




    for (const auto& dirEntry : recursive_directory_iterator("./tempdocxparsing")) {
        cout << dirEntry << endl;
    }

    // remove the temporary directory we created
    system("rm -rf ./tempdocxparsing");
}


