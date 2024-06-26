/*
 * Kye Jocham
*/

#include "drafts.hpp"

// Fucntion to create the directory for drafts
int create_draft_dir() {
    if (fs::exists(DRAFTS_DIR)) {
        return -1;
    } 

    if (!fs::create_directories(DRAFTS_DIR)) {
        return -2;
    }
    return 0;
}

// Funciton to create a draft
int create_draft(std::string draft_name, PlaceholderPair inputs) {
    // Check for directory
    if (!fs::exists(DRAFTS_DIR)) {
        if (create_draft_dir() != 0) {
            return -1;
        }
    } 

    // Open the draft file
    std::ofstream draft_file(DRAFTS_DIR / fs::path(draft_name).replace_extension(".json"));
    if (!draft_file.is_open()) {
        return -2;
    }

    // Input info as .json file
    json draft_vector;
    for (const auto& pair : inputs) {
        draft_vector[pair.first] = pair.second;
    }


    // Add input to file
    draft_file << draft_vector.dump(4);
    draft_file.close();

    return 0;
}

// Function to use a draft
PlaceholderPair use_draft(std::string draft_name) {
    // Check for directory
    if (!fs::exists(DRAFTS_DIR)) {
        if (create_draft_dir() != 0) {
            return {{"error", {"-1"}}};
        }
    } 
    
    // Open the json file
    std::ifstream draft_file(DRAFTS_DIR / fs::path(draft_name).replace_extension(".json"));
    if (!draft_file.is_open()) {
        return {{"error", {"-2"}}};
    }
    
    // Fill vector with json file info
    json draft_vector;
    draft_file >> draft_vector;
    draft_file.close();

    // Convert json object to vector
    PlaceholderPair inputs;
    for (const auto& item : draft_vector) {
        inputs.push_back(std::make_pair(item[0], item[1]));
    }

    if (delete_draft(draft_name) != 0) {
        return {{"error", {"-3"}}};
    }
    return inputs;
}

// Function to delete a draft
int delete_draft(std::string draft_name) {
    if (fs::exists(DRAFTS_DIR)) {
        return -1;
    } 

    if (fs::exists(DRAFTS_DIR / fs::path(draft_name).replace_extension(".json"))) {
        fs::remove(DRAFTS_DIR / fs::path(draft_name).replace_extension(".json"));
    } else {
        return -2;
    }
    return 0;
}