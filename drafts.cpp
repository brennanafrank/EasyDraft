#include "drafts.hpp"

int create_draft_dir() {
    if (fs::exists(DRAFTS_DIR)) {
        return -1;
    } 

    if (!fs::create_directories(DRAFTS_DIR)) {
        return -2;
    }
    return 0;
}

int create_draft(std::string draft_name, ValueVector inputs) {
    if (!fs::exists(DRAFTS_DIR)) {
        if (create_draft_dir() != 0) {
            return -1;
        }
    } 

    std::ofstream draft_file(DRAFTS_DIR / draft_name);
    if (!draft_file.is_open()) {
        return -2;
    }

    

    return 0;
}

int use_draft() {
    if (!fs::exists(DRAFTS_DIR)) {
        if (create_draft_dir() != 0) {
            return -1;
        }
    } 


    return 0;
}

int delete_draft() {
    if (!fs::exists(DRAFTS_DIR)) {
        if (create_draft_dir() != 0) {
            return -1;
        }
    } 


    return 0;
}