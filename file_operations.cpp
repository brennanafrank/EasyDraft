#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#define CURRENT_PATH fs::current_path()

namespace fs = std::filesystem;

// This funciton is used to save documents that are taken from
// the parser.
int save_document(std::string filename) {
    // Open file in arg for reading
    std::ifstream file(CURRENT_PATH / filename);
    if (!file.is_open()) {
        return -1;
    }

    // See if the folder for saved files is created
    fs::path folder_out = CURRENT_PATH / "\\output";
    if (!fs::exists(folder_out)) {
        if (!fs::create_directory(folder_out)) {
            // Could not create the directory
            file.close();
            return -2;
        }
    }

    // Open file in output folder for writing
    std::ofstream file_out(folder_out);
    if (!file_out.is_open()) {
        file.close();
        return -3;
    }

    // Read the contents from file and write to file_out
    std::string line;
    while (std::getline(file, line)) {
        file_out << line << std::endl;
    }

    //Close both files
    file.close();
    file_out.close();

    std::cout << "File " << filename << " saved successfully in " << folder_out;
    return 0;
}