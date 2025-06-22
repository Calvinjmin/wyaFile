#include "CommandParser.h"
#include "../core/Indexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <set>

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

    // Initialize directories to scan
    const char* home_env = std::getenv("HOME");
    if (home_env) {
        home_dir = std::string(home_env);
        directories_to_scan.push_back(home_dir + "/Documents");
        directories_to_scan.push_back(home_dir + "/Desktop");
    }
    
    // Add examples directory - prefer current directory examples
    directories_to_scan.push_back("../examples");
}

void CommandParser::addFlag(const std::string& flag) {
    flags.insert(flag);
}

bool CommandParser::hasFlag(const std::string& flag) {
    return flags.find(flag) != flags.end();
}

void CommandParser::clearFlags() {
    flags.clear();
}

std::string CommandParser::getFlagValue(const std::string& flag, const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == flag && i + 1 < args.size()) {
            // Check if the next argument is not another flag
            std::string next_arg = args[i + 1];
            if (next_arg[0] != '-' && next_arg[0] != '/') {
                return next_arg;
            }
        }
    }
    return "";
}

std::string CommandParser::parseCommand(const std::string& input) {
    std::vector<std::string> tokens = tokenizeCommand(input);

    if (tokens.empty()) {
        return "Please enter a command. Type 'help' for available commands.";
    }

    // Clear any previous flags
    clearFlags();
    
    // Extract flags from tokens (anything starting with - or --)
    std::vector<std::string> command_tokens;
    for (const auto& token : tokens) {
        if (token[0] == '-') {
            addFlag(token);
        } else {
            command_tokens.push_back(token);
        }
    }

    if (command_tokens.empty()) {
        return "Please enter a command. Type 'help' for available commands.";
    }

    std::string command = command_tokens[0];
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
    result << "\n";
    result << "Directory Scan: " << directory_path << "\n";
    result << "Found " << file_contents.size() << " file(s)\n";
    result << std::string(60, '-') << "\n\n";
    
    // Display contents of each file
    for (const auto& [filepath, content] : file_contents) {
        // Extract just the filename for cleaner display
        std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);
        
        result << "File: " << filename << "\n";
        result << std::string(40, '-') << "\n";
        
        // Show file content with line numbers
        std::stringstream content_stream(content);
        std::string line;
        int line_number = 1;
        
        while (std::getline(content_stream, line)) {
            result << std::setw(3) << std::setfill(' ') << line_number << " | " << line << "\n";
            line_number++;
        }
        
        result << "\n";
    }
    
    result << "Scan complete\n";
    
    return result.str();
}

std::string CommandParser::handleKeyCommand(const std::string& keyword) {    
    // Scan multiple directories for files
    Indexer indexer;
    std::map<std::string, std::string> all_file_contents;
    std::vector<std::string> scanned_directories;
    
    // Loop through all directories to scan
    for (const auto& directory : directories_to_scan) {
        std::map<std::string, std::string> dir_contents = indexer.scanDirectory(directory);
        
        // Merge results from this directory
        all_file_contents.insert(dir_contents.begin(), dir_contents.end());
        
        // Keep track of which directories were successfully scanned
        if (!dir_contents.empty()) {
            scanned_directories.push_back(directory);
        }
    }
    
    if (all_file_contents.empty()) {
        return "No files found in any of the search directories.";
    }
    
    std::stringstream result;
    result << "\n";
    result << "Keyword Search Results\n";
    result << std::string(50, '=') << "\n\n";
    result << "Searching for: \"" << keyword << "\"\n";
    result << "Directories: ";
    
    // List all scanned directories
    for (size_t i = 0; i < scanned_directories.size(); ++i) {
        if (i > 0) result << ", ";
        result << scanned_directories[i];
    }
    result << "\n\n";
    
    std::vector<std::string> matching_files;
    std::set<std::string> seen_filenames;
    std::set<std::string> seen_contents;
    
    // Search through each file for the keyword and deduplicate
    for (const auto& [filepath, content] : all_file_contents) {
        // Convert both content and keyword to lowercase for case-insensitive search
        std::string lower_content = content;
        std::string lower_keyword = keyword;
        std::transform(lower_content.begin(), lower_content.end(), lower_content.begin(), ::tolower);
        std::transform(lower_keyword.begin(), lower_keyword.end(), lower_keyword.begin(), ::tolower);
        
        // Check if keyword is found in the content
        if (lower_content.find(lower_keyword) != std::string::npos) {
            // Extract filename for deduplication
            std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);
            
            // Skip if we've already seen this filename or content
            if (seen_filenames.find(filename) != seen_filenames.end() || 
                seen_contents.find(content) != seen_contents.end()) {
                continue;
            }
            
            // Add to deduplication sets
            seen_filenames.insert(filename);
            seen_contents.insert(content);
            matching_files.push_back(filepath);
        }
    }
    
    if (matching_files.empty()) {
        result << "No files found containing \"" << keyword << "\"\n\n";
    } else {
        result << "Found " << matching_files.size() << " matching file(s):\n\n";
        
        // Display matching files with color highlighting
        for (size_t i = 0; i < matching_files.size(); ++i) {
            // Extract just the filename for cleaner display
            std::string filename = matching_files[i].substr(matching_files[i].find_last_of("/\\") + 1);
            
            // Add ANSI color codes for green text
            result << "  " << (i + 1) << ". \033[32m" << filename << "\033[0m\n";
            result << "      Path: " << matching_files[i] << "\n";
        }
        
        result << "\n";
    }
    
    result << "Search complete\n";
    
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
        // Check if --allow flag is present using the flag system
        if (!hasFlag("--allow")) {
            return "ERROR: Directory access requires --allow flag.\n"
                   "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow";
        }
        
        // Check for -key and -dir flags
        bool has_key_flag = hasFlag("-key");
        bool has_dir_flag = hasFlag("-dir");
        
        if (has_key_flag && has_dir_flag) {
            return "ERROR: Cannot use both -key and -dir flags together.\n"
                   "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow";
        }
        
        if (!has_key_flag && !has_dir_flag) {
            return "ERROR: Must specify either -key or -dir flag.\n"
                   "Usage: scan -key <keyword> --allow OR scan -dir <path> --allow";
        }
        
        if (has_key_flag) {
            std::string keyword = getFlagValue("-key", args);
            if (keyword.empty()) {
                return "ERROR: Missing keyword after -key flag.\n"
                       "Usage: scan -key <keyword> --allow";
            }
            return handleKeyCommand(keyword);
        }
        
        if (has_dir_flag) {
            std::string directory_path = getFlagValue("-dir", args);
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