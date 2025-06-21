#include "Indexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <filesystem>
#include <iostream>

namespace wyaFile {

Indexer::Indexer() {
    supported_extensions = {".txt", ".csv"};
}

std::string Indexer::readFileContent(const std::string& filepath) const {
    if (!isSupportedFile(filepath)) {
        return "";
    }

    if ( !fileExists(filepath) ) {
        return "";
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to convert text into a list of lowercase alphanumeric "words".
std::vector<std::string> Indexer::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::string current_token;
    
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current_token += std::tolower(static_cast<unsigned char>(c));
        } else {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

bool Indexer::isSupportedFile(const std::string& filepath) const {
    // Check if the file has a supported extension
    if ( filepath.length() < 4 ) {
        return false;
    }

    const std::string extension = filepath.substr(filepath.length() - 4 );
    return std::find(supported_extensions.begin(), supported_extensions.end(), extension) != supported_extensions.end();
}

bool Indexer::fileExists(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    return std::filesystem::exists(path);
}

std::map<std::string, std::string> Indexer::scanDirectory(const std::string& directory_path) const {
    std::map<std::string, std::string> file_contents;
    
    try {
        std::filesystem::path dir_path(directory_path);
        
        // Check if the directory exists
        if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
            std::cerr << "Error: Directory does not exist or is not a directory: " << directory_path << std::endl;
            return file_contents;
        }
        
        // Iterate through all files in the directory
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                
                // Check if it's a .txt file
                if (isSupportedFile(filepath)) {
                    std::string content = readFileContent(filepath);
                    if (!content.empty()) {
                        file_contents[filepath] = content;
                        std::cout << "Successfully read: " << entry.path().filename().string() << std::endl;
                    } else {
                        std::cout << "Could not read: " << entry.path().filename().string() << std::endl;
                    }
                }
            }
        }
        
        if (file_contents.empty()) {
            std::cout << "No supported files found in directory: " << directory_path << std::endl;
        } else {
            std::cout << "Found " << file_contents.size() << " supported files in directory." << std::endl;
        }
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }
    
    return file_contents;
}

} // namespace wyaFile