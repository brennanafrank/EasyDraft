#include "file_operations.hpp"

// This function is used to save uploaded templates to the 
// correct location.
int upload_template(fs::path filepath) {
    // Find the file extension to make sure its .docx
    if (!filepath.string().find(".docx")) {
        return -1;
    }

    // Open file from filepath
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return -2;
    }
    file.close();

    // Get filename from filepath
    //std::string filename = filepath.string().substr(filepath.string().find_last_of('\\'));
    //filename = filename.stem();
    std::string filename = (filepath.stem()).string();

    // See if the folder for uploaded templates is created
    fs::path import_dir = IMPORT_DIR;
    fprintf(stderr, "path: %s\n", (import_dir.string().c_str()));
    if (!fs::exists(import_dir)) {
        if (!fs::create_directory(import_dir)) {
            // Could not create the directory
            //file.close();
            return -3;
        }
    }

    // Create the folder for the template
    import_dir = import_dir / filename;
    fprintf(stderr, "folder name: %s\n", (import_dir).string().c_str());
    if (!fs::exists(import_dir)) {
        if (!fs::create_directory(import_dir)) {
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
    fs::path file_location = import_dir / filepath.filename();
    fprintf(stderr, "Filename path: %s\n", file_location.string().c_str());
    
    // Copy the file with overwrite_existing option
    std::error_code ec;
    fs::copy_file(filepath, file_location, ec);

    if (ec) {
        std::cerr << "Error copying file: " << ec.message() << std::endl;
        return -6;
    }

    // Close both files
    //file.close();
    //file_upload.close();

    std::cout << "File " << filename << " uploaded successfully" << std::endl;
    return 0;
}


// This function is used to delete a template directory and 
// all of the contents.
int delete_template(std::string filename) {
    // See if the directory exists
    fs::path dir_path = IMPORT_DIR / "\\" / filename.substr(0, filename.find_last_of('.'));
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
    std::ifstream file(CURRENT_DIR / filename);
    if (!file.is_open()) {
        return -1;
    }

    // See if the folder for saved files is created
    fs::path folder_out = EXPORT_DIR;
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


int docx_convert(std::string filename, std::string file_ext) {
    if (CONVERTER.c_str() == nullptr) {
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

// Used for testing
// /*
int main() {
    err_count = 0;
    fprintf(stderr, "err_count: %d\n", err_count); err_count++;
    read_config();
    fprintf(stderr, "err_count: %d\n", err_count); err_count++;

    fprintf(stderr, "Return: %d\n", upload_template("C:\\Users\\School\\Documents\\EasyDraft\\example_docx\\CS307 Charter.docx"));
        fprintf(stderr, "err_count: %d\n", err_count); err_count++;

    fprintf(stderr, "Return: %d\n", upload_template("C:\\Users\\School\\Documents\\EasyDraft\\example_docx\\CS307 Homework 2 Team 37.docx"));
    
        fprintf(stderr, "err_count: %d\n", err_count); err_count++;

    fprintf(stderr, "Return: %d\n", upload_template("C:\\Users\\School\\Documents\\EasyDraft\\example_docx\\Design Document Team 37.docx"));
    // upload_template("C:\\Users\\School\\Documents\\EasyDraft\\example_docx\\Design Document Team 37.docx");
        fprintf(stderr, "err_count: %d\n", err_count); err_count++;

    fprintf(stderr, "Return: %d\n", upload_template("C:\\Users\\School\\Documents\\EasyDraft\\example_docx\\Team 37 - Sprint #1 - Planning Document.docx"));
    // upload_template("C:\\Users\\School\\Documents\\EasyDraft\\example_docx\\Team 37 - Sprint #1 - Planning Document.docx");
        fprintf(stderr, "err_count: %d\n", err_count); err_count++;


    std::vector<std::string> templates = template_list();

    for (const auto& directory : templates) {
        std::cout << directory << std::endl;
    }

    return 0;
}
// */