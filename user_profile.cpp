#include <iostream>


void createProfile() {
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
            //create a new file
            //while loop to get user input
            std::ofstream outfile;
            outfile.open(fileName, std::ios_base::app);
            
            while (true) {
                std::string word;
                std::cout << "Enter a new placeholder: ";
                std::cin >> word;
                outfile << word << std::endl;
                
                std::cout << "Enter value for placeholder: ";
                std::cin >> word;
                outfile << word << std::endl;
                
                char choice;
                std::cout << "Continue? (y/n): ";
                std::cin >> choice;

                if (choice == 'n' || choice == 'N') {
                    break;
                }
            }
        }
}

int main() {
    return 0;
}
