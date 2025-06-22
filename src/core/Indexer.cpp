#include "Indexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace wyaFile {

Indexer::Indexer() {
    supported_extensions = {".txt", ".csv"};
    
    // Directories to skip for performance and relevance
    skip_directories = {
        ".git", ".svn", "node_modules", ".cache", ".DS_Store",
        "Library", "System", "Applications", "bin", "sbin",
        "tmp", "temp", "Downloads", "Trash", ".Trash",
        "Pictures", "Music", "Movies", "Videos", "Public",
        "Desktop", "Documents/Downloads", "Documents/Desktop",
        "AppData", "Application Support", "Preferences",
        "Caches", "Logs", "Saved Application State"
    };
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

bool Indexer::shouldSkipDirectory(const std::string& dirname) const {
    for (const auto& skip_pattern : skip_directories) {
        if (dirname.find(skip_pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool Indexer::shouldSkipFile(const std::string& filepath) const {
    // Skip files larger than 1MB
    try {
        std::filesystem::path path(filepath);
        if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
            size_t file_size = std::filesystem::file_size(path);
            if (file_size > 1024 * 1024) { // 1MB limit
                return true;
            }
        }
    } catch (...) {
        // If we can't check file size, skip it to be safe
        return true;
    }
    return false;
}

std::map<std::string, std::string> Indexer::scanDirectoryRecursive(const std::string& directory_path, int current_depth, int max_depth) const {
    std::map<std::string, std::string> file_contents;
    
    // Stop if we've reached max depth
    if (current_depth >= max_depth) {
        return file_contents;
    }
    
    try {
        std::filesystem::path dir_path(directory_path);
        
        // Check if the directory exists
        if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
            return file_contents;
        }
        
        // Iterate through all files and directories
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                
                // Skip files that are too large or not supported
                if (shouldSkipFile(filepath) || !isSupportedFile(filepath)) {
                    continue;
                }
                
                std::string content = readFileContent(filepath);
                if (!content.empty()) {
                    file_contents[filepath] = content;
                }
            }
            else if (entry.is_directory()) {
                std::string dirname = entry.path().filename().string();
                
                // Skip irrelevant directories
                if (shouldSkipDirectory(dirname)) {
                    continue;
                }
                
                std::string dirpath = entry.path().string();
                std::map<std::string, std::string> dir_contents = scanDirectoryRecursive(dirpath, current_depth + 1, max_depth);
                file_contents.insert(dir_contents.begin(), dir_contents.end());
            }
        }
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error scanning " << directory_path << ": " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory " << directory_path << ": " << e.what() << std::endl;
    }
    
    return file_contents;
}

std::map<std::string, std::string> Indexer::scanDirectory(const std::string& directory_path) const {
    // Use recursive scanning with guardrails (max depth of 3)
    return scanDirectoryRecursive(directory_path, 0, 5);
}

} // namespace wyaFile