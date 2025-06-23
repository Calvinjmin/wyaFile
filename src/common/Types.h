#ifndef WYAFILE_TYPES_H
#define WYAFILE_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <set>

namespace wyaFile {

// File content mapping: filepath -> content
using FileContents = std::map<std::string, std::string>;

// Directory scan result: directory -> file contents
using DirectoryResult = std::pair<std::string, FileContents>;

// Search results: list of matching file paths
using SearchResults = std::vector<std::string>;

// File extensions list
using FileExtensions = std::vector<std::string>;

// Directory skip list
using SkipDirectories = std::vector<std::string>;

// Command arguments
using CommandArgs = std::vector<std::string>;

// Command flags
using CommandFlags = std::set<std::string>;

// Command descriptions
using CommandDescriptions = std::map<std::string, std::string>;

} // namespace wyaFile

#endif // WYAFILE_TYPES_H 