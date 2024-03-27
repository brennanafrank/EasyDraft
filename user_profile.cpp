#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

//convert to user json files, to immplement with Micheal's json files.


typedef std::vector<std::vector<std::string>> ValueVector;

void createProfile(ValueVector placeholders, std::string fileName) {
    //create a new profile
    //check if file exists
    if (FILE *file = fopen(fileName.c_str(), "r")) {
        std::cout << "User profile already exists. Do you want to add new placeholders? (y/n): "; //change to qt message box
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

        fclose(file);
    }
}
void updatePlaceholder(std::string fileName, std::string placeholder, std::string value) {
    std::ifstream file(fileName);
    std::ofstream tempFile("temp.txt");
    std::string line;
    int reached = 0;
    while (std::getline(file, line)) {
        if (line == placeholder) {
            tempFile << line << std::endl;
            tempFile << value << std::endl;
            reached = 1;
        } else {
            tempFile << line << std::endl;
        }

    }
    if (reached == 0) {
        std::cout << "Placeholder not found" << std::endl; //change to qt message box
        return;
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

ValueVector getProfile(std::string fileName) {
    std::ifstream file(fileName);
    std::string line;
    ValueVector placeholders;
    int i = 0;
    while (std::getline(file, line)) {
        placeholders[i][0] = line;
        if (std::getline(file, line))
            placeholders[i][1];
        i++;
    }
    return placeholders;
}

//maybe not needed
void fillPlaceholders(std::string fileName, std::vector<std::vector<std::string>> placeholders ) {
    ValueVector profilePlaceholders = getProfile(fileName);
    std::string choice;
    

}



