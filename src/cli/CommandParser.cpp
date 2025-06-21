#include "CommandParser.h"
#include "../core/Indexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstdlib>

namespace wyaFile {

CommandParser::CommandParser() {
    initializeCommands();
}

void CommandParser::initializeCommands() {
    // Initialize command descriptions
    command_descriptions["scan"] = "Scan a directory for .txt files and display their contents";
    command_descriptions["key"] = "Scan the 'examples' directory for relevant documents for the given keyword";
    command_descriptions["help"] = "Show available commands and their descriptions";
    command_descriptions["exit"] = "Exit the application";

    // ARG COMMANDS
    arg_commands["scan"] = &CommandParser::handleScanCommandWithArgs;
    arg_commands["key"] = &CommandParser::handleKeyCommandWithArgs;

    // NO ARG COMMANDS
    no_arg_commands["help"] = &CommandParser::handleHelpCommand;
    no_arg_commands["exit"] = &CommandParser::handleExitCommand;
}

std::string CommandParser::parseCommand(const std::string& input) {
    std::vector<std::string> tokens = tokenizeCommand(input);

    if (tokens.empty()) {
        return "Please enter a command. Type 'help' for available commands.";
    }

    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    // Check arg commands first
    auto arg_it = arg_commands.find(command);
    if (arg_it != arg_commands.end()) {
        return (this->*(arg_it->second))(tokens);
    }

    // Check no-arg commands
    auto no_arg_it = no_arg_commands.find(command);
    if (no_arg_it != no_arg_commands.end()) {
        return (this->*(no_arg_it->second))();
    }

    return handleUnknownCommand(command);
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

std::string CommandParser::handleKeyCommand(const std::string& keyword) {
    // Get examples directory from environment variable or use default
    const char* env_examples_dir = std::getenv("EXAMPLES_DIR");
    std::string examples_dir = env_examples_dir ? env_examples_dir : "../examples";
    
    // Scan the examples directory for files
    Indexer indexer;
    std::map<std::string, std::string> file_contents = indexer.scanDirectory(examples_dir);
    
    if (file_contents.empty()) {
        return "No files found in the examples directory.";
    }
    
    std::stringstream result;
    result << "\n=== Keyword Search Results ===\n";
    result << "Searching for keyword: '" << keyword << "'\n\n";
    
    std::vector<std::string> matching_files;
    
    // Search through each file for the keyword
    for (const auto& [filepath, content] : file_contents) {
        // Convert both content and keyword to lowercase for case-insensitive search
        std::string lower_content = content;
        std::string lower_keyword = keyword;
        std::transform(lower_content.begin(), lower_content.end(), lower_content.begin(), ::tolower);
        std::transform(lower_keyword.begin(), lower_keyword.end(), lower_keyword.begin(), ::tolower);
        
        // Check if keyword is found in the content
        if (lower_content.find(lower_keyword) != std::string::npos) {
            matching_files.push_back(filepath);
        }
    }
    
    if (matching_files.empty()) {
        result << "No files found containing the keyword '" << keyword << "'.\n";
    } else {
        result << "Found " << matching_files.size() << " file(s) containing '" << keyword << "':\n\n";
        
        for (const auto& filepath : matching_files) {
            result << "  - " << filepath << "\n";
            
            // Show the relevant content with keyword highlighted
            std::string content = file_contents[filepath];
            std::string lower_content = content;
            std::string lower_keyword = keyword;
            std::transform(lower_content.begin(), lower_content.end(), lower_content.begin(), ::tolower);
            std::transform(lower_keyword.begin(), lower_keyword.end(), lower_keyword.begin(), ::tolower);
            
            // Find the position of the keyword
            size_t pos = lower_content.find(lower_keyword);
            if (pos != std::string::npos) {
                // Show context around the keyword (up to 100 characters)
                size_t start = (pos > 50) ? pos - 50 : 0;
                size_t end = std::min(pos + keyword.length() + 50, content.length());
                std::string context = content.substr(start, end - start);
                
                result << "    Context: ..." << context << "...\n";
            }
            result << "\n";
        }
    }
    
    result << "=== End of Search Results ===\n";
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
    help << "  key <keyword> - Scan the examples directory for <keyword> input\n";
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

// Helper methods for function pointers
std::string CommandParser::handleScanCommandWithArgs(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: scan <directory_path>\nExample: scan /path/to/directory";
    }
    return handleScanCommand(args[1]);
}

std::string CommandParser::handleKeyCommandWithArgs(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: key <keyword>";
    }
    return handleKeyCommand(args[1]);
}

} // namespace wyaFile