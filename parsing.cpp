// DocumentModifier.cpp
#include "parsing.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

void removeEmptyValues(std::vector<std::pair<std::string, std::vector<std::string>>>& replacements) {
    size_t maxSize = 0;

    for (const auto& p : replacements) {
        maxSize = std::max(maxSize, p.second.size());
    }

    for (size_t i = 0; i < maxSize; ++i) {
        bool allEmpty = true;

        for (const auto& p : replacements) {
            if (i < p.second.size() && !p.second[i].empty()) {
                allEmpty = false;
                break;
            }
        }

        if (allEmpty) {
            for (auto& p : replacements) {
                if (i < p.second.size()) {
                    p.second.erase(p.second.begin() + i);
                }
            }
            --i;
            --maxSize;
        }
    }
}

std::string vectorToJson(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements) {
    auto replacementsCopy = replacements;
    removeEmptyValues(replacementsCopy);
    nlohmann::json j;
    for (const auto& p : replacementsCopy) {
        j[p.first] = p.second;
    }
    return j.dump();
}

std::vector<std::pair<std::string, std::vector<std::string>>> jsonToVector(const std::string& jsonStr) {
    nlohmann::json j = nlohmann::json::parse(jsonStr);
    std::vector<std::pair<std::string, std::vector<std::string>>> replacements;

    for (auto& el : j.items()) {
        replacements.push_back({el.key(), el.value().get<std::vector<std::string>>()});
    }

    return replacements;
}

void saveJsonToFile(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements, const std::string& filepath) {
    auto replacementsCopy = replacements;
    removeEmptyValues(replacementsCopy);

    nlohmann::json j;
    for (const auto& p : replacementsCopy) {
        j[p.first] = p.second;
    }

    std::ofstream outputFile(filepath);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filepath);
    }
    outputFile << j.dump();
    outputFile.close();
}


std::vector<std::pair<std::string, std::vector<std::string>>> readJsonFromFile(const std::string& filepath) {
    std::ifstream inputFile(filepath);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filepath);
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    inputFile.close();

    nlohmann::json j = nlohmann::json::parse(buffer.str());
    std::vector<std::pair<std::string, std::vector<std::string>>> replacements;

    for (auto& el : j.items()) {
        replacements.push_back({el.key(), el.value().get<std::vector<std::string>>()});
    }
    return replacements;
}


void modifyDocument(const std::string& docPath, const std::string& replacementsJson) {
    std::string tempJsonPath = "./tempreplacements.json";

    std::ofstream outFile(tempJsonPath);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << tempJsonPath << std::endl;
        return;
    }
    outFile << replacementsJson;
    outFile.close();

    std::string command = "/Users/michael/anaconda3/bin/python /Users/michael/Developer/EasyDraft/modify_docx.py \"" + docPath + "\" \"" + tempJsonPath + "\"";

    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Python script failed with exit code " << result << std::endl;
    }

    std::remove(tempJsonPath.c_str());
}

std::vector<std::pair<std::string, std::vector<std::string>>> findPlaceholdersInDocument(const std::string& docPath) {
    std::string command = "/Users/michael/anaconda3/bin/python /Users/michael/Developer/EasyDraft/findPlaceholders.py \"" + docPath + "\"";
    std::array<char, 128> buffer;
    std::string result;


    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return jsonToVector(result);
}
