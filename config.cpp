#include "config.hpp"

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

}

// Function to create config.json and input all
// information needed for the program to run.
int create_fill_config() {
    std::ofstream config_file("config.json", std::ios::out);
    if (!config_file.is_open()) {
        return -1;
    }

    return 0;
}

int change_config() {
    return 0;
}