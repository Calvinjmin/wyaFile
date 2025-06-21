#ifndef WYAFILE_COMMANDPARSER_H
#define WYAFILE_COMMANDPARSER_H

#include <string>
#include <map>
#include <vector>

namespace wyaFile {

class CommandParser {
public:
    // Constructor
    CommandParser();
    
    // Main command parsing method
    std::string parseCommand(const std::string& input);
    
    // Individual command handlers
    std::string handleScanCommand(const std::string& directory_path);
    std::string handleHelpCommand();
    std::string handleExitCommand();
    std::string handleUnknownCommand(const std::string& command);
    
    // Utility methods
    std::vector<std::string> tokenizeCommand(const std::string& input);
    bool isValidCommand(const std::string& command);
    
    // Get available commands
    std::vector<std::string> getAvailableCommands() const;

private:
    std::map<std::string, std::string> command_descriptions;
    void initializeCommands();
};

} // namespace wyaFile

#endif // WYAFILE_COMMANDPARSER_H