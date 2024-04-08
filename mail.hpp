#ifndef GMAIL_H
#define GMAIL_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// Gmail API functions
inline int GM_authenticate() {
    return std::system("python gmail.py authenticate");
}

inline int GM_send_document(std::string recipient, std::string subject, std::string message, std::string filepath) {
    return std::system(std::string("python gmail.py send_document \"" + recipient + "\" \"" + subject + "\" \"" + message + "\" \"" + filepath + "\"").c_str());
}

int GM_bulk_send_document(std::vector<std::string> recipients, std::vector<std::string> subjects, std::vector<std::string> messages, std::vector<std::string> filepaths) {
    if (recipients.size() != subjects.size() || recipients.size() != messages.size() || recipients.size() != filepaths.size()) {
        std::cerr << "Error: All argument vectors must have the same length." << std::endl;
        return -1;
    }

    for (size_t i = 0; i < recipients.size(); ++i) {
        int result = std::system(std::string("python gmail.py send_document \"" + recipients[i] + "\" \"" + subjects[i] + "\" \"" + messages[i] + "\" \"" + filepaths[i] + "\"").c_str());
        if (result != 0) {
            std::cerr << "Error: Failed to send document to " << recipients[i] << std::endl;
            return result;
        }
    }
    return 0;
}

#endif /* GMAIL_H */