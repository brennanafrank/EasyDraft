// parsing.h
#ifndef PARSING_H
#define PARSING_H

#include <string>
#include <vector>
#include <QProcess>
#include <QDebug>

#include "nlohmann/json.hpp"

void removeEmptyValues(std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);

std::string vectorToJson(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);

std::vector<std::pair<std::string, std::vector<std::string>>> jsonToVector(const std::string& jsonStr);

void callPythonScript(const std::string& docPath, const std::string& replacementsPath);

void modifyDocument(const std::string& docPath, const std::string& replacementsJson, const QString& savePath, const QString& fileNamePrefix);

void saveJsonToFile(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements, const std::string& filepath);

std::vector<std::pair<std::string, std::vector<std::string>>> readJsonFromFile(const std::string& filepath);

std::vector<std::pair<std::string, std::vector<std::string>>> findPlaceholdersInDocument(const std::string& docPath);

#endif // PARSING_H
