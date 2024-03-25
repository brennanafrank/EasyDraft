// parsing.h
#ifndef PARSING_H
#define PARSING_H

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

std::string vectorToJson(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);
std::vector<std::pair<std::string, std::vector<std::string>>> jsonToVector(const std::string& jsonStr);
void callPythonScript(const std::string& docPath, const std::string& replacementsPath);
void modifyDocument(const std::string& docPath, const std::string& replacementsJson, const std::string& tempJsonPath = "./replacements.json");

#endif // PARSING_H
