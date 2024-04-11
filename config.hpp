#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <iostream>
#include <filesystem>
#include <ctime>
#include "json.hpp"
#include <QString>
#include <vector>
#include <chrono>

using json = nlohmann::json;
namespace fs = std::filesystem;

// Define a custom type for a vector of integers
typedef std::vector<std::pair<std::string, std::vector<std::string>>> PlaceholderPair;

// Inline global variables for paths
inline fs::path CURRENT_DIR = (fs::current_path()).string();
inline fs::path IMPORT_DIR = (fs::current_path().append("templates")).string();
inline fs::path EXPORT_DIR = "/Users/aneeshpendyala/Desktop/ExportTemplates";
inline fs::path DRAFTS_DIR = (fs::current_path().append("drafts")).string();
inline fs::path LOG_DIR = (fs::current_path().append("logs")).string();
inline const std::string PYTHON_EXEC_PATH = "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3";
inline const std::string PROJECT_PATH = "/Users/aneeshpendyala/Desktop/EasyDraft";
inline const QString TAG_MANAGER_JSON_PATH = QString::fromStdString((fs::current_path() / "tag.json").string());
inline const QString TEMPLATES_PATH = "/Users/aneeshpendyala/Desktop";

// Inline global variables for misc
inline std::string CONVERTER = "None";
inline std::chrono::hours LOG_DELETE_TIME(24); // One day


// Function to create config.json and input all
// information needed for the program to run.
inline int create_fill_config() {
    std::ofstream config_file("config.json");
    if (!config_file.is_open()) {
        return -1;
    }

    // Input default info
    json default_config = {
        {"paths", {
                      {"CURRENT_DIR", (fs::current_path()).string()},
                      {"IMPORT_DIR", (fs::current_path().append("templates")).string()},
                      {"EXPORT_DIR", (fs::current_path().append("output")).string()},
                      {"DRAFTS_DIR", (fs::current_path().append("drafts")).string()}
                  }},
        {"misc", {
                     {"CONVERTER", "None"}
                 }}
    };

    config_file << default_config.dump(4) << std::endl;
    config_file.close();

    return 0;
}


// Function to read the configuration file and
// fill out the variables that have values in
// the config file. This would be used on startup.
inline int read_config() {
    // Check if file exists
    if (!fs::exists(fs::current_path().append("config.json"))) {
        if (create_fill_config() != 0) {
            return -1;
        }
    }

    // Open config file
    std::ifstream config_file(fs::current_path().append("config.json"));
    if (!config_file.is_open()) {
        return -2;
    }

    // Parse config file to JSON object
    json config;
    config_file >> config;
    if (config_file.fail()) {
        return -3;
    }
    config_file.close();

    // Fill out variables with config info here
    // Paths
    CURRENT_DIR = static_cast<fs::path>(config["paths"]["CURRENT_DIR"]);
    IMPORT_DIR = static_cast<fs::path>(config["paths"]["IMPORT_DIR"]);
    EXPORT_DIR = static_cast<fs::path>(config["paths"]["EXPORT_DIR"]);
    DRAFTS_DIR = static_cast<fs::path>(config["paths"]["DRAFTS_DIR"]);

    // Misc
    CONVERTER = static_cast<std::string>(config["misc"]["CONVERTER"]);

    return 0;
}


// Function to change the value of a configuration
// in the file.
inline int change_config(std::string key, std::string value, bool json_refresh) {
    if (key == "CURRENT_DIR") {
        CURRENT_DIR = value;
    } else if (key == "IMPORT_DIR") {
        IMPORT_DIR = value;
    } else if (key == "EXPORT_DIR") {
        EXPORT_DIR = value;
    } else if (key == "DRAFTS_DIR") {
        DRAFTS_DIR = value;
    } else if (key == "CONVERTER") {
        CONVERTER = value;
    } else if (key == "n/a") {

    } else {
        return -1;
    }

    if (json_refresh) {
        std::string config_filename = "config.txt";
        if (fs::exists(config_filename)) {
            fs::remove(config_filename);
        }

        json config = {
            {"paths", {
                          {"CURRENT_DIR", CURRENT_DIR},
                          {"IMPORT_DIR", IMPORT_DIR},
                          {"EXPORT_DIR", EXPORT_DIR},
                          {"DRAFTS_DIR", DRAFTS_DIR}
                      }},
            {"misc", {
                         {"CONVERTER", CONVERTER}
                     }}
        };

        std::ofstream config_file("config.json");
        if (!config_file.is_open()) {
            return -2;
        }

        config_file << config.dump(4) << std::endl;
        config_file.close();
    }
    return 0;
}

// Function to reset the config file to defaults.
inline int reset_config() {
    std::string config_file = "config.txt";

    if (fs::exists(config_file)) {
        fs::remove(config_file);
        return create_fill_config();
    } else {
        return -2;
    }
}

// Avoid using extern unless you really need to link to variables defined elsewhere
extern int err_count;

#endif /* CONFIG_HPP */
