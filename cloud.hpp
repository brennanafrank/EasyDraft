#ifndef CLOUD_H
#define CLOUD_H

#include <cstdlib>
#include <string>

// Google Drive API functions
inline int GD_authenticate() {
    return std::system("python3 google_drive1.py authenticate");
}

inline int GD_create_easydraft() {
    return std::system("python3 google_drive1.py create_easydraft");
}

inline int GD_select_template(std::string file_path) {
    return std::system((std::string("python3 google_drive1.py select_template \"" + file_path + "\"")).c_str());
}

inline int GD_download_template(std::string file_name, std::string local_directory) {
    return std::system((std::string("python3 google_drive1.py download_template \"" + file_name + "\" \"" + local_directory + "\"")).c_str());
}

inline int GD_upload_document(std::string file_name, std::string local_directory) {
    return std::system((std::string("python3 google_drive1.py upload_document \"" + file_name + "\" \"" + local_directory + "\"")).c_str());
}

inline int GD_delete_file(std::string file_name, std::string folder_name) {
    return std::system(std::string("python3 google_drive1.py delete_file \"" + file_name + "\" \"" + folder_name + "\"").c_str());
}

inline int GD_delete_easydraft() {
    return std::system("python3 google_drive1.py delete_easydraft");
}

#endif /* CLOUD_H */
