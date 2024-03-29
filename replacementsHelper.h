#ifndef REPLACEMENTSHELPER_H
#define REPLACEMENTSHELPER_H
#include <string>
#include <vector>

bool isReplacementsValid(const std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);
void removeEmptyValuesFromReplacements(std::vector<std::pair<std::string, std::vector<std::string>>>& replacements);

#endif // REPLACEMENTSHELPER_H
