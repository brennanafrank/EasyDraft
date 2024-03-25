// DocumentModifier.cpp
#include "parsing.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

std::string vectorToJson(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements) {
    nlohmann::json j;
    for (const auto& p : replacements) {
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

void callPythonScript(const std::string& docPath, const std::string& replacementsPath) {
    std::string command = "python modify_docx.py " + docPath + " " + replacementsPath;
    system(command.c_str());
}

void modifyDocument(const std::string& docPath, const std::string& replacementsJson, const std::string& tempJsonPath) {
    std::ofstream outFile(tempJsonPath);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << tempJsonPath << std::endl;
        return;
    }
    outFile << replacementsJson;
    outFile.close();

    callPythonScript(docPath, tempJsonPath);

    std::remove(tempJsonPath.c_str());
}
