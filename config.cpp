#include "config.hpp"

int err_count;

// Defining global variables for config.hpp.
// Use if using read_config and config.json
/*
// "paths"
fs::path CURRENT_DIR;
fs::path IMPORT_DIR;
fs::path EXPORT_DIR;

// "misc"
std::string CONVERTER;
*/

// Use if not using read_config and config.json.
// This is used to make testing easier. 
// Will still need to compile config.cpp for program to run.
fs::path CURRENT_DIR = (fs::current_path()).string();
fs::path IMPORT_DIR = (fs::current_path().append("templates")).string();
//fs::path EXPORT_DIR = (fs::current_path().append("output")).string();
fs::path EXPORT_DIR = ("/Users/aneeshpendyala/Desktop/ExportTemplates");
fs::path DRAFTS_DIR = (fs::current_path().append("drafts")).string();
std::string CONVERTER = "None";

// Function to read the configuration file and
// fill out the variables that have values in
// the config file. This would be used on startup.
int read_config() {
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

// Function to create config.json and input all
// information needed for the program to run.
int create_fill_config() {
    std::ofstream config_file("config.json");
    if (!config_file.is_open()) {
        return -1;
    }

    // Input defualt info
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

// Function to change the value of a configuration
// in the file.
int change_config(std::string key, std::string value, bool json_refresh) {
    if (key == "CURRENT_DIR") {    
        CURRENT_DIR = value;
    } else if (key == "IMPORT_DIR") {
        IMPORT_DIR = value;
    } else if (key == "EXPORT_DIR") {
        EXPORT_DIR = value;
    } else if (key == "DRAFT_DIR") {
        DRAFTS_DIR = value;
    } else if (key == "CONVERTER") {
        CONVERTER = value;
    } else if (key == "n/a") {
        
    } else {
        return -1;
    }
    return 0;

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
                {"DRAFT_DIR", DRAFTS_DIR}
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

// Funciton to reset the config file to defaults.
int reset_config() {
    std::string config_file = "config.txt";

    if (fs::exists(config_file)) {
        fs::remove(config_file);
        return create_fill_config();
    } else {
        return -2;
    }
}
