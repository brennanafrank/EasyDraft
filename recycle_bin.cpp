#include "recycle_bin.hpp"

int move_template_to_trash(std::string filename) {
    // Function to move a file to the trash directory
    // Open file in arg for reading
    
    std::string filename_ext = filename + ".docx";
    fs::path filepath = fs::current_path().parent_path().string();
    std::cout << filepath << std::endl;
    if (!fs::exists(filepath)) {
        // File does not exist
        return -1;
    }
    // Check if the trash directory exists
    if (!fs::exists(TRASH_DIR)) {
        if (!fs::create_directory(TRASH_DIR)) {
            // Could not create the directory

            return -1;
        }
    }

    // Construct the full destination file path
    fs::path destination_file = TRASH_DIR / filename_ext;
    try {
        std::filesystem::rename(filepath, destination_file);
    } catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

int restore_template(std::string filename) {
    std::string filename_ext = filename + ".docx";
    fs::path filepath = TRASH_DIR / filename_ext;
    
    if (!fs::exists(filepath)) {
        // File does not exist
        return -1;
    }

    // Construct the full destination file path
    fs::path destination_file = IMPORT_DIR / filename_ext;
    try {
        std::filesystem::rename(filepath, destination_file);
    } catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
