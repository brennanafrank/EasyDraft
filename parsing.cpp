#include "./tinyxml2/tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>

using namespace tinyxml2;

void ReplacePlaceholders(XMLNode* node, const char* placeholder, const char* replacement) {
    for (XMLNode* child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
        if (child->ToElement() && strcmp(child->Value(), "w:t") == 0) {
            XMLText* textNode = child->FirstChild()->ToText();
            if (textNode && strcmp(textNode->Value(), placeholder) == 0) {
                textNode->SetValue(replacement);
            }
        }
        ReplacePlaceholders(child, placeholder, replacement);
    }
}



void ReplaceInDocument(const char* filePath, const std::vector<std::pair<std::string, std::string>>& replacements) {
    XMLDocument doc;
    if (doc.LoadFile(filePath) == XML_SUCCESS) {
        for (const auto& pair : replacements) {
            std::string fullPlaceholder = "$$" + pair.first + "$$";
            ReplacePlaceholders(&doc, fullPlaceholder.c_str(), pair.second.c_str());
        }
        doc.SaveFile(filePath);
    } else {
        std::cerr << "Failed to load document: " << filePath << std::endl;
    }
}


int main() {
    const char* filePath = "./test_doc/word/document.xml";

    std::vector<std::pair<std::string, std::string>> replacements = {
        {"Name", "Michael"},
        {"Address", "HK"}
    };

    XMLDocument doc;
    doc.LoadFile(filePath);

    ReplaceInDocument(filePath, replacements);

    return 0;
}
