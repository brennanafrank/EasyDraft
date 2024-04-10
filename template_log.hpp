
 * Kye Jocham
*/

#ifndef TEMPLATE_LOG_H
#define TEMPLATE_LOG_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "config.hpp"
#include "log.hpp"
#include "json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

int start_template_log();
int add_template_log(std::string);
int remove_template_log(std::string);
int add_template_use(std::string);

int start_template_log() {
    // Make sure template_log.json exists
    // Try to open the existing JSON file
    std::fstream log_file("template_log.json", std::ios::binary | std::ios::ate);
    if (log_file.is_open()) {
        // Check if the file is empty
        std::streamsize size = log_file.tellg();
        if (size > 0) {
            log_file.close();
            return 0;
        }
    }

    // Input defualt info
    json empty_log = {};

    log_file << empty_log.dump(4) << std::endl;
    log_file.close();

    return 0;
}

int add_template_log(std::string file_name) {
    json entry;

    // Try to open the existing JSON file
    std::ifstream log_file("template_log.json");
    if (log_file.is_open()) {
        // Read the existing JSON data
        try {
            log_file >> entry;
        } catch (json::parse_error& e) {
            // Handle JSON parsing error (e.g., if the file is empty or corrupted)
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            log_file.close();
            return -3;
        }
        log_file.close();
    }

    // Add or update the template entry with the value 0
    entry[file_name] = 0;

    // Write the updated JSON data back to the file
    std::ofstream log_file_write("template_log.json");
    if (!log_file_write.is_open()) {
        std::cerr << "Failed to open template_log.json for writing." << std::endl;
        return -2;
    }
    log_file_write << entry.dump(4); // Pretty-print with an indentation of 4 spaces
    log_file_write.close();
    return 0;
}

int remove_template_log(std::string file_name) {
    json entry;

    // Try to open the existing JSON file
    std::ifstream log_file("template_log.json");
    if (log_file.is_open()) {
        try {
            log_file >> entry;
        } catch (json::parse_error& e) {
            // Handle JSON parsing error (e.g., if the file is corrupted)
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            log_file.close();
            return -2;
        }
        log_file.close();
    } else {
        std::cerr << "Failed to open template_log.json for reading." << std::endl;
        return -3;
    }

    // Check if the template exists and remove it
    if (entry.find(file_name) != entry.end()) {
        entry.erase(file_name);

        // Write the updated JSON data back to the file
        std::ofstream log_file_write("template_log.json");
        if (!log_file_write.is_open()) {
            std::cerr << "Failed to open template_log.json for writing." << std::endl;
            return -4;
        }
        log_file_write << entry.dump(4); // Pretty-print with an indentation of 4 spaces
        log_file_write.close();
        return 0;
    } else {
        return -1; // Template not found
    }
}

int add_template_use(std::string file_name) {
    json entry;

    // Try to open the existing JSON file
    std::ifstream log_file("template_log.json");
    if (!log_file.is_open()) {
        std::cerr << "Failed to open template_log.json for reading." << std::endl;
        return -3;
    }

    // Read the JSON data
    try {
        log_file >> entry;
    } catch (json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        log_file.close();
        return -2;
    }
    log_file.close();

    // Check if the template exists and increment its use count
    if (entry.contains(file_name)) {
        entry[file_name] = entry[file_name].get<int>() + 1;

        // Write the updated JSON data back to the file
        std::ofstream log_file_write("template_log.json");
        if (!log_file_write.is_open()) {
            std::cerr << "Failed to open template_log.json for writing." << std::endl;
            return -4;
        }
        log_file_write << entry.dump(4); // Pretty-print with an indentation of 4 spaces
        log_file_write.close();
        return 0;
    } else {
        return -1; // Template not found
    }
}

/*
namespace fs = std::filesystem;

std::string log_file_name;

int start_log();
int remove_logs();
void log(std::string);
void lerror(std::string);

// Set up logs at beginning of program running
int start_log() {
    // Create log directory if it doesn't exist
    if (!fs::exists(LOG_DIR)) {
        if (!fs::create_directory(LOG_DIR)) {
            return -1;
        }
    }

    // Check if any log files need to be deleted
    remove_logs();

    // Open the new log file
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H-%M-%S");
    log_file_name = ss.str() + ".txt";

    std::ofstream log_file(LOG_DIR / log_file_name);
    if (!log_file.is_open()) {
        return -2;
    }

    log_file.close();
    return 0;
}

// Remove log files that are out of date
int remove_logs() {
    namespace fs = std::filesystem;
    auto now = std::chrono::system_clock::now();
    int remove_count = 0;

    try {
        for (const auto& entry : fs::directory_iterator(LOG_DIR)) {
            if (entry.is_regular_file()) {
                // Deletion is based on the last time the file was written to, not based on the name
                auto file_time = fs::last_write_time(entry);

                // Manual conversion from file_time_type to system_clock::time_point
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    file_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                );

                auto file_age = now - sctp;

                if (file_age > LOG_DELETE_TIME) {
                    fs::remove(entry);
                    remove_count++;
                    std::cout << "Removed: " << entry.path() << std::endl;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return remove_count;
}

// Add a log to the log file
void log(std::string message) {
    // Get the current date and time
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    // Format the date and time
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

    // Open the log file
    std::ofstream log_file(LOG_DIR / log_file_name, std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << log_file_name << std::endl;
        return;
    }

    // Write the log message
    log_file << "log: " << ss.str() << "\n" << message << "\n\n";

    // Close the log file
    log_file.close();
}

// Add an error to the log file
void lerror(std::string error) {
    // Get the current date and time
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    // Format the date and time
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

    // Open the log file
    std::ofstream log_file(LOG_DIR / log_file_name, std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << log_file_name << std::endl;
        return;
    }

    // Write the log message
    log_file << "error: " << ss.str() << "\n" << error << "\n\n";

    // Close the log file
    log_file.close();
}
*/
#endif /* TEMPLATE_LOG_H
