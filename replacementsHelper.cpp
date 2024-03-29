#ifndef REPLACEMENTSHELPER_CPP
#define REPLACEMENTSHELPER_CPP

#include "replacementsHelper.h"

bool isReplacementsValid(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements) {
    for (const auto& pair : replacements) {
        for (const std::string& value : pair.second) {
            if (value.empty()) {
                return false;
            }
        }
    }
    return true;
}

void removeEmptyValuesFromReplacements(std::vector<std::pair<std::string, std::vector<std::string>>>& replacements) {
    bool emptyPage = false;
    size_t maxSize = 0;

    // Find the maximum size of the inner vectors
    for (const auto& pair : replacements) {
        maxSize = std::max(maxSize, pair.second.size());
    }

    // Check each index
    for (size_t i = 0; i < maxSize; ++i) {
        emptyPage = true;

        // Check if all values at this index are empty
        for (auto& pair : replacements) {
            if (i < pair.second.size() && !pair.second[i].empty()) {
                emptyPage = false;
                break;
            }
        }

        // If all values at this index are empty, remove them
        if (emptyPage) {
            for (auto& pair : replacements) {
                if (i < pair.second.size()) {
                    pair.second.erase(pair.second.begin() + i);
                }
            }
            --maxSize;
            --i; // Decrement i to check the same index again after removal
        }
    }
}
#endif // REPLACEMENTSHELPER_CPP
