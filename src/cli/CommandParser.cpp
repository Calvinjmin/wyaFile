#include "CommandParser.h"
#include "../core/Indexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <iomanip>

namespace wyaFile {

CommandParser::CommandParser() {
    initializeCommands();
}

void CommandParser::initializeCommands() {
    // Initialize command descriptions
    command_descriptions["scan"] = "Scan operations: use -key <keyword> for keyword search or -dir <path> for directory scan (requires --allow flag)";
    command_descriptions["help"] = "Show available commands and their descriptions";
    command_descriptions["exit"] = "Exit the application";

    // ARG COMMANDS
    arg_commands["scan"] = &CommandParser::handleCommandWithArgs;

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
        return "❌ No .txt files found or could not access directory: " + directory_path;
    }
    
    std::stringstream result;
    result << "\n";
    result << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    result << "║                           DIRECTORY SCAN RESULTS                             ║\n";
    result << "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
    result << "📁 Directory: " << directory_path << "\n";
    result << "📊 Found " << file_contents.size() << " .txt file(s)\n\n";
    
    // Display contents of each file
    size_t file_count = 0;
    for (const auto& [filepath, content] : file_contents) {
        file_count++;
        
        result << "┌─ File " << file_count << " of " << file_contents.size() << " ──────────────────────────────────────────────┐\n";
        result << "📄 " << filepath << "\n";
        result << "└─────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        // Show file content with line numbers
        std::stringstream content_stream(content);
        std::string line;
        int line_number = 1;
        
        result << "📝 File contents:\n";
        result << "┌─────────────────────────────────────────────────────────────────────────────┐\n";
        
        while (std::getline(content_stream, line)) {
            result << "│ " << std::setw(3) << std::setfill(' ') << line_number << " │ " << line << "\n";
            line_number++;
        }
        
        result << "└─────────────────────────────────────────────────────────────────────────────┘\n\n";
    }
    
    result << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    result << "║                              END OF SCAN RESULTS                             ║\n";
    result << "╚══════════════════════════════════════════════════════════════════════════════╝\n";
    
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
    result << "\n";
    result << "+" << std::string(78, '=') << "+\n";
    result << "|                           KEYWORD SEARCH RESULTS                             |\n";
    result << "+" << std::string(78, '=') << "+\n\n";
    result << "Searching for keyword: \"" << keyword << "\"\n";
    result << "Search directory: " << examples_dir << "\n\n";
    
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
        result << "No files found containing the keyword \"" << keyword << "\"\n\n";
    } else {
        result << "Found " << matching_files.size() << " file(s) containing \"" << keyword << "\":\n\n";
        
        // Display matching files in a clean list format
        for (size_t i = 0; i < matching_files.size(); ++i) {
            result << "  " << (i + 1) << ". " << matching_files[i] << "\n";
        }
        
        result << "\n";
    }
    
    result << "+" << std::string(78, '=') << "+\n";
    result << "|                              END OF SEARCH RESULTS                           |\n";
    result << "+" << std::string(78, '=') << "+\n";
    
    return result.str();
}

std::string CommandParser::handleHelpCommand() {
    std::stringstream help;
    help << "\n=== wyaFile Command Help ===\n";
    help << "Available commands:\n\n";
    
    for (const auto& [command, description] : command_descriptions) {
        help << "  " << command << " - " << description << "\n";
    }
    
    help << "\nSecurity Notice:\n";
    help << "  Directory scanning requires the --allow flag for security.\n";
    help << "  This flag must be included with each scan operation.\n\n";
    
    help << "Examples:\n";
    help << "  scan -key <keyword> --allow       - Search examples directory for keyword\n";
    help << "  scan -dir /path/to/directory --allow - Scan directory for .txt files\n";
    help << "  exit                                    - Exit the application\n";
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
std::string CommandParser::handleCommandWithArgs(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow\n"
               "Examples:\n"
               "  scan -key basic --allow\n"
               "  scan -dir /path/to/directory --allow";
    }
    
    std::string command = args[0];
    
    if (command == "scan") {
        // Check if --allow flag is present anywhere in the arguments
        bool has_access_flag = false;
        bool has_key_flag = false;
        bool has_dir_flag = false;
        std::string keyword;
        std::string directory_path;
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--allow") {
                has_access_flag = true;
            } else if (args[i] == "-key") {
                has_key_flag = true;
                // Get the keyword that follows -key
                if (i + 1 < args.size() && args[i + 1] != "--allow" && args[i + 1] != "-dir") {
                    keyword = args[i + 1];
                    i++; // Skip the next argument since we've consumed it
                }
            } else if (args[i] == "-dir") {
                has_dir_flag = true;
                // Get the directory path that follows -dir
                if (i + 1 < args.size() && args[i + 1] != "--allow" && args[i + 1] != "-key") {
                    directory_path = args[i + 1];
                    i++; // Skip the next argument since we've consumed it
                }
            }
        }
        
        if (!has_access_flag) {
            return "ERROR: Directory access requires --allow flag.\n"
                   "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow";
        }
        
        if (has_key_flag && has_dir_flag) {
            return "ERROR: Cannot use both -key and -dir flags together.\n"
                   "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow";
        }
        
        if (!has_key_flag && !has_dir_flag) {
            return "ERROR: Must specify either -key or -dir flag.\n"
                   "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow";
        }
        
        if (has_key_flag) {
            if (keyword.empty()) {
                return "ERROR: Missing keyword after -key flag.\n"
                       "Usage: scan -key <keyword> --allow";
            }
            return handleKeyCommand(keyword);
        }
        
        if (has_dir_flag) {
            if (directory_path.empty()) {
                return "ERROR: Missing directory path after -dir flag.\n"
                       "Usage: scan -dir <path> --allow";
            }
            return handleScanCommand(directory_path);
        }
    }
    
    return handleUnknownCommand(command);
}

} // namespace wyaFile