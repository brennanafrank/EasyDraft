#include <iostream>
#include <cstdio>

void move_to_trash(char* filename) {
    // Function to move a file to the trash directory

    std::string trash_path = CURRENT_DIR.string() + "/trash/" + filename;

    if (std::rename(filename, trash_path.c_str()) != 0) {
        std::cout << "Error moving file to trash" << std::endl;
    } else {
        std::cout << "File moved to trash successfully" << std::endl;
    }

    return;
}

void permanent_delete(char* filename) {
     // Function to delete a template from the current directory

    if (std::remove(filename) != 0) {
        std::cout << "Error deleting file" << std::endl;
    } else {
        std::cout << "File deleted successfully" << std::endl;
    }

    return;
}
