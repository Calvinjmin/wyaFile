#include "Indexer.h"
#include "CommandParser.h"
#include <iostream>
#include <string>

int main() {
    wyaFile::CommandParser commandParser;

    std::cout << "Welcome to wyaFile Directory Scanner.\n";
    std::cout << "Type 'help' for available commands or 'exit' to quit.\n\n";

    std::string user_input;
    while (true) {
        std::cout << "wyaFile> ";
        std::getline(std::cin, user_input);

        if (user_input.empty()) {
            continue;
        }

        std::string result = commandParser.parseCommand(user_input);
        
        if (result == "EXIT_COMMAND") {
            std::cout << "Exiting wyaFile. Goodbye!\n";
            break;
        } else {
            std::cout << result << std::endl;
        }
    }

    return 0;
}