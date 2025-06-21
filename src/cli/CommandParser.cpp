#include "CommandParser.h"
#include "../core/Indexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace wyaFile {

CommandParser::CommandParser() {
    initializeCommands();
}

void CommandParser::initializeCommands() {
    command_descriptions["scan"] = "Scan a directory for .txt files and display their contents";
    command_descriptions["help"] = "Show available commands and their descriptions";
    command_descriptions["exit"] = "Exit the application";
    command_descriptions["quit"] = "Exit the application";
}

std::string CommandParser::parseCommand(const std::string& input) {
    std::vector<std::string> tokens = tokenizeCommand(input);
    
    if (tokens.empty()) {
        return "Please enter a command. Type 'help' for available commands.";
    }
    
    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "exit" || command == "quit") {
        return handleExitCommand();
    } else if (command == "help") {
        return handleHelpCommand();
    } else if (command == "scan") {
        if (tokens.size() < 2) {
            return "Usage: scan <directory_path>\nExample: scan /path/to/directory";
        }
        return handleScanCommand(tokens[1]);
    } else {
        return handleUnknownCommand(command);
    }
}

std::string CommandParser::handleScanCommand(const std::string& directory_path) {
    Indexer indexer;
    std::map<std::string, std::string> file_contents = indexer.scanDirectory(directory_path);
    
    if (file_contents.empty()) {
        return "No .txt files found or could not access directory: " + directory_path;
    }
    
    std::stringstream result;
    result << "\n=== Directory Scan Results ===\n";
    result << "Found " << file_contents.size() << " .txt file(s) in: " << directory_path << "\n\n";
    
    // Display contents of each file
    for (const auto& [filepath, content] : file_contents) {
        result << "--- File: " << filepath << " ---\n";
        result << content << std::endl;
        result << "--- End of File ---\n\n";
    }
    result << "=== End of Directory Scan ===\n";
    
    return result.str();
}

std::string CommandParser::handleHelpCommand() {
    std::stringstream help;
    help << "\n=== wyaFile Command Help ===\n";
    help << "Available commands:\n\n";
    
    for (const auto& [command, description] : command_descriptions) {
        help << "  " << command << " - " << description << "\n";
    }
    
    help << "\nExamples:\n";
    help << "  scan /path/to/directory  - Scan a directory for .txt files\n";
    help << "  exit                     - Exit the application\n";
    help << "===========================\n";
    
    return help.str();
}

std::string CommandParser::handleExitCommand() {
    return "EXIT_COMMAND";
}

std::string CommandParser::handleUnknownCommand(const std::string& command) {
    return "Unknown command: '" + command + "'. Type 'help' for available commands.";
}

std::vector<std::string> CommandParser::tokenizeCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool CommandParser::isValidCommand(const std::string& command) {
    std::string lower_command = command;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(), ::tolower);
    
    return command_descriptions.find(lower_command) != command_descriptions.end() ||
           lower_command == "quit";
}

std::vector<std::string> CommandParser::getAvailableCommands() const {
    std::vector<std::string> commands;
    for (const auto& [command, _] : command_descriptions) {
        commands.push_back(command);
    }
    commands.push_back("quit"); // Add quit as an alias for exit
    return commands;
}

} // namespace wyaFile