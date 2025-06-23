#ifndef NEXUSSCAN_INDEXER_H
#define NEXUSSCAN_INDEXER_H

#include "../common/Types.h"

namespace wyaFile {

class Indexer {
private:
    FileExtensions supported_extensions;
    SkipDirectories skip_directories;
    
    // Recursive scanning with guardrails
    FileContents scanDirectoryRecursive(const std::string& directory_path, int current_depth, int max_depth) const;
    bool shouldSkipDirectory(const std::string& dirname) const;
    bool shouldSkipFile(const std::string& filepath) const;

public:
    Indexer();

    std::string readFileContent(const std::string& filepath) const;
    std::vector<std::string> tokenize(const std::string& text) const;
    
    // New method to scan directory and read all .txt files
    FileContents scanDirectory(const std::string& directory_path) const;
    
    // Helper method to check if a file has .txt extension
    bool isSupportedFile(const std::string& filepath) const;
    bool fileExists(const std::string& filepath) const;
};

} // namespace wyaFile

#endif // NEXUSSCAN_INDEXER_H