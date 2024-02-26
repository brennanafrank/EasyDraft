//
// Created by Brennan Frank on 2/18/24.
//

#include <TemplateParser.hpp>


void TemplateParser::setReplacements(const vector<string>& replacements) {
    this->replacements = replacements;
}

TemplateParser::TemplateParser() {}

// the file path to our docx/zip file has to be in full
void TemplateParser::parse(string fullFilePath) {
    cout << "here1 \n";
    fs::path tempdocxparsing("./tempdocxparsing");
    cout << "here1 \n";
    fs::create_directory(tempdocxparsing);
    cout << "here1 \n";

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

    miniz_cpp::zip_file input(fullFilePath); // load zipFileName
    cout << "here2\n";
    cout << input.get_filename() << " \n";
    input.printdir(cout);
    cout << "here3\n";

    mz_zip_archive zip_archive;




    /* later */
    for (const auto& dirEntry : recursive_directory_iterator("./tempdocxparsing")) {
        cout << dirEntry << endl;
    }

    // remove the temporary directory we created
    system("rm -rf ./tempdocxparsing"); // #TODO make system independent
}


