#ifndef DRAFTS_H
#define DRAFTS_H

#include <filesystem>

#include "config.hpp"

namespace fs = std::filesystem;

int create_draft_dir();
int create_draft(std::string, PlaceholderPair);
int use_draft();
int delete_draft();

#endif /* FILE_OPERATIONS_H */