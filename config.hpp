#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

// Define a custom type for a vector of integers
typedef std::vector<std::pair<std::string, std::string>> PlaceholderPair;

int read_config();
int create_fill_config();
int change_config(std::string, std::string, bool);
int reset_config();

// Global variables from config.json
// "paths"
extern fs::path CURRENT_DIR;
extern fs::path IMPORT_DIR;
extern fs::path EXPORT_DIR;
extern fs::path DRAFTS_DIR;

// "misc"
extern std::string CONVERTER;

extern int err_count;

#endif /* CONFIG_H */