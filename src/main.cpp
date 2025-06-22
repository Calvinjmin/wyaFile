#include "cli/CommandParser.h"
#include <iostream>
#include <string>

int main() {
    wyaFile::CommandParser commandParser;

    std::cout << "Welcome to wyaFile Directory Scanner.\n";
    std::cout << "Type 'help' for available commands or 'exit' to quit.\n\n";
    
    std::cout << "SECURITY NOTICE: Any scanning operations require the --allow flag.\n";
    std::cout << "Examples: scan -key basic --allow OR scan -dir /path --allow\n\n";

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