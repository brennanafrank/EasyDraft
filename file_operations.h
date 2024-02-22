#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#define CURRENT_PATH std::filesystem::current_path()

namespace fs = std::filesystem;

int upload_template(fs::path filepath);

int delete_template(std::string filename);

int save_document(std::string filename);

#endif /* FILE_OPERATIONS_H */