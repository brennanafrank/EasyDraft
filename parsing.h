// parsing.h
#ifndef PARSING_H
#define PARSING_H

#include <string>
#include <vector>

// 使用nlohmann/json库，确保你的项目能找到它
#include "nlohmann/json.hpp"

// 将std::vector<std::pair<std::string, std::vector<std::string>>>转换为JSON字符串
std::string vectorToJson(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);

// 将JSON字符串转换回std::vector<std::pair<std::string, std::vector<std::string>>>
std::vector<std::pair<std::string, std::vector<std::string>>> jsonToVector(const std::string& jsonStr);

// 调用Python脚本修改文档
void callPythonScript(const std::string& docPath, const std::string& replacementsPath);

// 修改文档的函数，包括生成临时JSON文件和调用Python脚本
void modifyDocument(const std::string& docPath, const std::string& replacementsJson, const std::string& tempJsonPath = "./replacements.json");

#endif // PARSING_H
