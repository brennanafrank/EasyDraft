#include "file_operations.h"

// This function is used to save uploaded templates to the 
// correct location.
int upload_template(fs::path filepath) {
    // Find the file extension to make sure its .docx
    if (!filepath.string().find(".docx")) {
        return -1;
    }

    // Open file from filepath
    std::fstream file(filepath);
    if (!file.is_open()) {
        return -2;
    }

    // Get filename from filepath
    std::string filename = filepath.string().substr(filepath.string().find_last_of('\\'));

    // See if the folder for uploaded templates is created
    fs::path folder_temps = CURRENT_PATH / "\\templates";
    if (!fs::exists(folder_temps)) {
        if (!fs::create_directory(folder_temps)) {
            // Could not create the directory
            file.close();
            return -3;
        }
    }

    // Create the folder for the template
    fs::path temp_folder = CURRENT_PATH / "\\templates" / "\\" / filename.substr(0, filename.find_last_of('.'));
    if (!fs::exists(temp_folder)) {
        if (!fs::create_directory(temp_folder)) {
            // Could not create the directory
            file.close();
            return -4;
        }
    } else {
        // Folder exists
        file.close();
        return -5;
    }

    // Open new tempalte in new folder
    std::fstream file_upload(temp_folder / filename);
    if (!file_upload.is_open()) {
        file.close();
        return -6;
    }

    // Read from file and write to file_upload
    std::string line;
    while (std::getline(file, line)) {
        file_upload << line << std::endl;
    }

    // Close both files
    file.close();
    file_upload.close();

    std::cout << "File " << filename << " uploaded successfully" << std::endl;
    return 0;
}


// This function is used to delete a template directory and 
// all of the contents.
int delete_template(std::string filename) {
    // See if the directory exists
    fs::path dir_path = CURRENT_PATH / "\\templates" / "\\" / filename.substr(0, filename.find_last_of('.'));
    if (!fs::exists(dir_path)) {
        return -1;
    }

    // Remove direcotry and all contents
    if (fs::remove_all(dir_path) > 0) {
        std::cout << filename.substr(0, filename.find_last_of('.')) << " deleted successfully." << std::endl;
        return 0;
    } else {
        return -2;
    }
}


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

    std::cout << "File " << filename << " saved successfully in " << folder_out << std::endl;
    return 0;
}