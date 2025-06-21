#ifndef NEXUSSCAN_INDEXER_H
#define NEXUSSCAN_INDEXER_H

#include <string>
#include <vector>
#include <map>

namespace wyaFile {

class Indexer {
private:
    std::vector<std::string> supported_extensions;

public:
    Indexer();

    std::string readFileContent(const std::string& filepath) const;
    std::vector<std::string> tokenize(const std::string& text) const;
    
    // New method to scan directory and read all .txt files
    std::map<std::string, std::string> scanDirectory(const std::string& directory_path) const;
    
    // Helper method to check if a file has .txt extension
    bool isSupportedFile(const std::string& filepath) const;
    bool fileExists(const std::string& filepath) const;
};

} // namespace wyaFile

#endif // NEXUSSCAN_INDEXER_H