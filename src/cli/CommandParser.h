#ifndef WYAFILE_COMMANDPARSER_H
#define WYAFILE_COMMANDPARSER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

namespace wyaFile {

class CommandParser {
public:
    // Constructor
    CommandParser();
    
    // Main command parsing method
    std::string parseCommand(const std::string& input);
    
    // Individual command handlers
    std::string handleScanCommand(const std::string& directory_path);
    std::string handleKeyCommand(const std::string& keyword);
    std::string handleHelpCommand();
    std::string handleExitCommand();
    std::string handleUnknownCommand(const std::string& command);
    
    // Get available commands
    std::vector<std::string> getAvailableCommands() const;

    private:
    // Variables
    std::string home_dir;
    std::set<std::string> flags;
    std::vector<std::string> directories_to_scan;

    // Utility methods
    std::vector<std::string> tokenizeCommand(const std::string& input);
    bool isValidCommand(const std::string& command);
    void addFlag(const std::string& flag);
    bool hasFlag(const std::string& flag);
    void clearFlags();
    std::string getFlagValue(const std::string& flag, const std::vector<std::string>& args);

    // Command Maps
    std::map<std::string, std::string> command_descriptions;
    std::map<std::string, std::string (CommandParser::*)(const std::vector<std::string>&)> arg_commands;
    std::map<std::string, std::string (CommandParser::*)()> no_arg_commands;
    void initializeCommands();
    
    // Helper methods for function pointers
    std::string handleCommandWithArgs(const std::vector<std::string>& args);
};

} // namespace wyaFile

#endif // WYAFILE_COMMANDPARSER_H