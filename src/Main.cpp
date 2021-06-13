#include <iostream>
#include <cstring>
#include <array>
#include <limits>

#include "Message.h"
#include "MovieDB.h"

int main(int argc, char* argv[]) {

    MovieDB* db = new MovieDB();

    std::array<std::string, 5> options = {
        "   1: Add a movie",
        "   2: List all movies",
        "   3: Delete a movie",
        "   4: Reset whole database",
        "   0: Exit"
        };

    std::cout << Message::WELCOME << std::endl;

    short selection, choice;
    while (true) {
        std::cout << "What would you like to do?" << std::endl;

        for (auto& option : options)
            std::cout << option << std::endl;

        std::cout << "Enter selection [0.." << options.size() - 1 << "] > ";
        try {
            std::cin >> selection;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw 1;
            }
            if (selection < 0 || selection >= options.size())
                throw 2;

            bool fail = false;
            switch (selection){
                case 0:
                    std::cout << Message::GOODBYE << std::endl;

                    delete db;

                    std::cout << "Press any key to exit...";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getchar();
                    return 0;

                case 1:
                    db->add();
                    break;

                case 2:
                    do {
                        std::cout << "   1: ID" << std::endl
                                  << "   2: Title" << std::endl
                                  << "   3: Year" << std::endl
                                  << "   4: Rating" << std::endl
                                  << "   0: Return" << std::endl
                                  << "List by [0..4] > ";
                        std::cin >> choice;
                       if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            fail = true;
                            std::cout << Message::WRONG_INPUT << std::endl;
                        } else {
                            fail = false;
                            if (choice < 0)
                                std::cout << Message::OUT_OF_RANGE << std::endl;
                            else
                                if (choice > 4)
                                    std::cout << Message::OUT_OF_RANGE << std::endl;
                        }
                    } while (fail || choice < 0 || choice > 4);
                    
                    if (choice) {
                        std::cout << std::endl;
                        db->list(choice);
                    }
                    std::cout << std::endl;
                    break;

                case 3:
                    do {
                        std::cout << "Movie ID [0: return] > ";
                        std::cin >> choice;
                       if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            fail = true;
                            std::cout << Message::WRONG_INPUT << std::endl;
                        } else {
                            fail = false;
                            if (choice < 0)
                                std::cout << Message::OUT_OF_RANGE << std::endl;
                        }
                    } while (fail || choice < 0);
                    
                    if (choice)
                        db->del(choice);
                    std::cout << std::endl;
                    break;
                    
                case 4:
                    db->reset();
                    std::cout
                        << std::endl
                        << "Database reset successfully!" << std::endl
                        << std::endl;
                    break;

                default:
                    break;
            }
        } catch (int ex) {
            switch (ex) {
                case 1:
                    std::cout << std::endl
                            << Message::WRONG_INPUT << std::endl
                            << std::endl;
                    break;
                case 2:
                    std::cout << std::endl
                            << Message::OUT_OF_RANGE << std::endl
                            << std::endl;
                    break;
                default:
                    break;
            }
        }
    }
}
