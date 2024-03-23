#include <iostream>
#include <cstdio>


void move_to_trash(char* filename) {
    // Function to move a file to the trash directory

    std::string trash_path =  "./trash/" + std::string(filename);

    if (std::rename(filename, trash_path.c_str()) != 0) {
        std::cout << "Error moving file to trash" << std::endl;
    } else {
        std::cout << "File moved to trash successfully" << std::endl;
    }

    return;
}

