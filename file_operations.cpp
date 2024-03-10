#include "file_operations.hpp"

// This function is used to save uploaded templates to the correct location.
int upload_template(fs::path filepath) {
    // Find the file extension to make sure it's .docx
    if (filepath.extension() != ".docx") {
        return -1;
    }

    // Get filename from filepath
    std::string filename = filepath.stem().string();

    // See if the folder for uploaded templates is created
    fs::path import_dir = IMPORT_DIR;
    std::cout << "Path: " << import_dir << std::endl;
    if (!fs::exists(import_dir)) {
        if (!fs::create_directory(import_dir)) {
            // Could not create the directory
            return -3;
        }
    }

    // Create the folder for the template
    import_dir /= filename;
    std::cout << "Folder name: " << import_dir << std::endl;
    if (!fs::exists(import_dir)) {
        if (!fs::create_directory(import_dir)) {
            // Could not create the directory
            return -4;
        }
    } else {
        // Folder exists
        return -5;
    }

    // Open new template in new folder
    fs::path file_location = import_dir / filepath.filename();
    std::cout << "Filename path: " << file_location << std::endl;

    // Copy the file with overwrite_existing option
    std::error_code ec;
    fs::copy_file(filepath, file_location, fs::copy_options::overwrite_existing, ec);

    if (ec) {
        std::cerr << "Error copying file: " << ec.message() << std::endl;
        return -6;
    }

    std::cout << "File " << filename << " uploaded successfully" << std::endl;
    return 0;
}


// This function is used to delete a template directory and 
// all of the contents.
int delete_template(std::string filename) {
    // See if the directory exists
    fs::path dir_path = IMPORT_DIR / filename.substr(0, filename.find_last_of('.'));
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
int export_document(std::string filename) {
    // Open file in arg for reading
    std::string filename_ext = filename + ".docx";
    fs::path filepath = IMPORT_DIR / filename / filename_ext;
    std::cout << "path: " << filepath << std::endl;

    // See if the folder for saved files is created
    fs::path folder_out = EXPORT_DIR;
    if (!fs::exists(folder_out)) {
        if (!fs::create_directory(folder_out)) {
            // Could not create the directory
            return -1;
        }
    }

    // Construct the full destination file path
    fs::path destination_file = folder_out / filename_ext;

    std::error_code ec;
    fs::copy_file(filepath, destination_file, fs::copy_options::overwrite_existing, ec);

    if (ec) {
        std::cerr << "Error copying file: " << ec.message() << std::endl;
        return -2;
    }

    std::cout << "File " << filename << " saved successfully in " << folder_out << std::endl;
    return 0;
}



std::vector<std::string> template_list() {
    std::vector<std::string> templates;

    for (const auto& entry : fs::directory_iterator(IMPORT_DIR)) {
        if (fs::is_directory(entry.path())) {
            templates.push_back(entry.path().filename().string());
        }
    }

    if (templates.empty()) {
        return {};
    }

    return templates;
}


// Function to search the vector to find templates
// that include the key value.
std::vector<std::string> search_template_list(std::string key) {
    std::vector<std::string> temp_list = template_list();
    if (temp_list.empty()) {
        return {};
    }
    std::vector<std::string> result_list;

    // Iterate over temp_list and add strings that start with key to result_list
    for (const std::string& str : temp_list) {
        if (str.find(key) == 0) {
            result_list.push_back(str);
        }
    }

    if (result_list.empty()) {
        return {};
    }
    return result_list;
}


int docx_convert(std::string filename, std::string file_ext) {
    if (CONVERTER == "None") {
        // User hasn't choosen a converter
        return -2;
    } else if (CONVERTER == "LibreOffice") {
        return libre_converter(filename, file_ext);
    } else if (CONVERTER == "Pandoc") {
        return pandoc_converter(filename, file_ext);
    } else {
        // Converter field is corrupt
        return -3;
    }
}

// This function uses LibreOffice to convert document types through
// the command line.
int libre_converter(std::string filename, std::string file_ext) {
    // Check if user has libreoffice
    if (std::system("libreoffice --version") != 0) {
        return -1;
    }

    // Run conversion command
    fs::path folder_out = EXPORT_DIR;
    return std::system(("libreoffice --headless --convert-to " + file_ext + " --outdir " + folder_out.string() + " " + filename).c_str());
}


// This function uses Pandoc to convert docuement types through
// the command line.
int pandoc_converter(std::string filename, std::string file_ext) {
    // Check if user has pandoc
    if (std::system("pandoc --version") != 0) {
        return -1;
    }

    // Run conversion command
    fs::path folder_out = EXPORT_DIR;
    return std::system(("pandoc -s " + filename + " -o " + filename.substr(0, filename.find_last_of('.')) + file_ext).c_str());
}