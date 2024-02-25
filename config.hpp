#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

int read_config();
int create_fill_config();
int change_config();

// Global variables from config.json
// "paths"
extern fs::path current_dir;
extern fs::path import_dir;
extern fs::path export_dir;

// "misc"
extern std::string converter;

#endif /* CONFIG_H */