#include "config.hpp"

// Function to read the configuration file and
// fill out the variables that have values in
// the config file. This would be used on startup.
int read_config() {
    // Check if file exists
    if (!fs::exists("config.json")) {
        if (create_fill_config() != 0) {
            return -1;
        }
    }

    // Open config file
    std::ifstream config_file("config.json");
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
    for (json::iterator it = config.begin(); it != config.end(); ++it) {

    }

    return 0;
}

// Function to create config.json and input all
// information needed for the program to run.
int create_fill_config() {
    //std::ofstream config_file("config.json", std::ios::out);
    std::ofstream config_file("config.json");
    if (!config_file.is_open()) {
        return -1;
    }

    // Input defualt info
    json default_config = {
        {"paths", {
            {"current_dir", nullptr},
            {"import_dir", nullptr},
            {"export_dir", nullptr}
        }},
        {"misc", {
            {"converter", nullptr}
        }}
    };

    config_file << default_config.dump(4) << std::endl;
    config_file.close();

    return 0;
}

// Function to change the value of a configuration
// in the file.
int change_config() {
    return 0;
}