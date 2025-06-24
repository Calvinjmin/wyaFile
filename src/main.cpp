#include "cli/CommandParser.h"
#include <iostream>

int main(int argc, char* argv[]) {
    wyaFile::CommandParser commandParser;

    // If no arguments provided, show help
    if (argc == 1) {
        std::cout << commandParser.handleHelpCommand() << std::endl;
        return 0;
    }

    // Combine all arguments into a single command string
    std::string command;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) command += " ";
        command += argv[i];
    }

    // Parse and execute the command
    std::string result = commandParser.parseCommand(command);
}