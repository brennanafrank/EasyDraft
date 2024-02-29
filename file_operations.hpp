#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "config.hpp"

namespace fs = std::filesystem;

int upload_template(fs::path filepath);
int delete_template(std::string filename);
int save_document(std::string filename);
std::vector<std::string> template_list();
std::vector<std::string> search_template_list();
int docx_convert(std::string filename, std::string file_ext);
int libre_converter(std::string filename, std::string file_ext);
int pandoc_converter(std::string filename, std::string file_ext);

#endif /* FILE_OPERATIONS_H */