/*
 * Kye Jocham
*/

#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "config.hpp"

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

#endif /* LOG_H */