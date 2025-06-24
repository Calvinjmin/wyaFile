#ifndef WYAFILE_COMMANDPARSER_H
#define WYAFILE_COMMANDPARSER_H

#include "../common/Types.h"
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
    std::string handleUnknownCommand(const std::string& command);
    
    // Get available commands
    std::vector<std::string> getAvailableCommands() const;

    private:
    // Variables
    std::string home_dir;
    CommandFlags flags;
    std::vector<std::string> directories_to_scan;

    // Utility methods
    CommandArgs tokenizeCommand(const std::string& input);
    bool isValidCommand(const std::string& command);
    void addFlag(const std::string& flag);
    bool hasFlag(const std::string& flag);
    void clearFlags();
    std::string getFlagValue(const std::string& flag, const CommandArgs& args);

    // Command Maps
    CommandDescriptions command_descriptions;
    std::map<std::string, std::string (CommandParser::*)(const CommandArgs&)> arg_commands;
    std::map<std::string, std::string (CommandParser::*)()> no_arg_commands;
    void initializeCommands();
    
    // Helper methods for function pointers
    std::string handleCommandWithArgs(const CommandArgs& args);
};

} // namespace wyaFile

#endif // WYAFILE_COMMANDPARSER_H