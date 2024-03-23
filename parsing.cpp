#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <vector>
#include <sstream>

std::string vectorToJson(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements) {
    std::stringstream json;
    json << "{";
    for (auto it = replacements.begin(); it != replacements.end(); ++it) {
        json << "\"" << it->first << "\": [";
        for (auto vecIt = it->second.begin(); vecIt != it->second.end(); ++vecIt) {
            json << "\"" << *vecIt << "\"";
            if (vecIt != it->second.end() - 1) json << ", ";
        }
        json << "]";
        if (it != replacements.end() - 1) json << ", ";
    }
    json << "}";
    return json.str();
}


void callPythonScript(const std::string& docPath, const std::string& replacementsPath) {
    std::string command = "python modify_docx.py " + docPath + " " + replacementsPath;
    system(command.c_str());
}

void modifyDocument(const std::string& docPath, const std::string& replacementsJson, const std::string& tempJsonPath = "./replacements.json") {
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

int main() {
    std::vector<std::pair<std::string, std::vector<std::string>>> replacements = {
        {"Name", {"Michael", "Sarah", "Alex",}},
        {"Address", {"NY", "LA", "HK"}},
        {"Phone", {"123456", "654321", "987654"}}
    };

    // Path to the document to be modified
    std::string docPath = "./test_doc.docx";

    modifyDocument(docPath, vectorToJson(replacements));

    return 0;
}
