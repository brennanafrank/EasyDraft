#include <iostream>
#include <fstream>
#include <unordered_map>


void createProfile(std::vector<std::vector<std::string>> placeholders) {
    //create a new profile
    std::string fileName;
    std::cout << "Enter a new profile name: ";
    std::cin >> fileName;

    //check if file exists
        if (FILE *file = fopen(fileName.c_str(), "r")) {
            std::cout << "User profile already exists. Do you want to add new placeholders? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'n' || choice == 'N') {
                fclose(file);
                return;
            }
            fclose(file);
        }
        else {
            
            std::ofstream outfile;
            outfile.open(fileName, std::ios_base::app);
            for (int i = 0; i < placeholders.size(); i++) {
                outfile << placeholders[i][0] << std::endl;
                outfile << placeholders[i][1] << std::endl;
            }
            
            fclose(outfile);
        }
}
void updatePlaceholders(std::string fileName, std::string placeholder, std::string value) {
    std::ifstream file(fileName);
    std::ofstream tempFile("temp.txt");
    std::string line;

    while (std::getline(file, line)) {
        if (line == placeholder) {
            tempFile << line << std::endl;
            tempFile << value << std::endl;
        } else {
            tempFile << line << std::endl;
        }

    }

    file.close();
    tempFile.close();

    std::remove(fileName.c_str());
    std::rename("temp.txt", fileName.c_str());
    
}

void deleteProfile(std::string fileName) {
    std::remove(fileName.c_str());
}

void addProfilePlaceholder(std::string fileName, std::string placeholder, std::string value) {
    std::ofstream outfile;
    outfile.open(fileName, std::ios_base::app);
    outfile << placeholder << std::endl;
    outfile << value << std::endl;
}

void fillPlaceholders(std::string fileName) {
    std::unordered_map<std::string, std::string> profilePlaceholders = getProfile(fileName);
    std::string choice;
    std::string placeholders[10];
    std::cout << "Do you want to autofill(1) or select placeholders to match(2)? (1/2): ";
    std::cin >> choice;
    if (choice == "1") {
        //autofill
        for (const auto& pair : profilePlaceholders) {
            //check each placeholder in profile if it matches one placeholder in placeholders array
            if (placeholders.find(pair.first) != placeholders.end()) {
                //find text box related to the placeholder

                //fill in the text box with the value in profile (pair.second)
                
            }
            else {
                //if it doesn't match, ask if user wants to add it to another text box

                //if yes 
            }
        }
        
    }
    else if (choice == "2") {
        //select placeholders
    }
    else {
        std::cout << "Invalid choice. Please enter 1 or 2." << std::endl;
    }

}


std::unordered_map<std::string, std::string> getProfile(const std::string& fileName) {
    std::unordered_map<std::string, std::string> dictionary;
    std::ifstream file(fileName);
    std::string line;
    int lineNumber = 1;

    while (std::getline(file, line)) {
        std::string key = line;
        std::getline(file, line);
        std::string value = line;
        dictionary[key] = value;
        
        lineNumber++;
    }

    return dictionary;
}

int main() {
    std::string fileName;
    std::cout << "Enter a file name: ";
    std::cin >> fileName;
    fillPlaceholders(fileName);
   

    

    
    return 0;
}
