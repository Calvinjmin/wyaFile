cmake_minimum_required(VERSION 3.16)

# Project name and version
project(wyaFile VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to 17 for filesystem support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Function to automatically discover source files
function(discover_sources SOURCE_DIR SOURCES_VAR HEADERS_VAR)
    file(GLOB_RECURSE ${SOURCES_VAR} 
        "${SOURCE_DIR}/*.cpp"
        "${SOURCE_DIR}/*.cc"
        "${SOURCE_DIR}/*.cxx"
    )
    file(GLOB_RECURSE ${HEADERS_VAR} 
        "${SOURCE_DIR}/*.h"
        "${SOURCE_DIR}/*.hpp"
        "${SOURCE_DIR}/*.hxx"
    )
    set(${SOURCES_VAR} ${${SOURCES_VAR}} PARENT_SCOPE)
    set(${HEADERS_VAR} ${${HEADERS_VAR}} PARENT_SCOPE)
endfunction()

# Discover source and header files
discover_sources(src SOURCES HEADERS)

# Create executable with name "wya"
add_executable(wya ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(wya PRIVATE src)

# Set compiler flags for better warnings and optimization
target_compile_features(wya PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(wya PRIVATE /W4)
else()
    target_compile_options(wya PRIVATE -Wall -Wextra -O2)
endif()

# Optional: Add installation rules
install(TARGETS wya
    RUNTIME DESTINATION bin
)

# Optional: Print some information
message(STATUS "Building wya version ${PROJECT_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source files: ${SOURCES}")
message(STATUS "Header files: ${HEADERS}")

# Optional: Add a custom target to list all source files
add_custom_target(list-sources
    COMMAND ${CMAKE_COMMAND} -E echo "Source files:"
    COMMAND ${CMAKE_COMMAND} -E echo "${SOURCES}"
    COMMAND ${CMAKE_COMMAND} -E echo "Header files:"
    COMMAND ${CMAKE_COMMAND} -E echo "${HEADERS}"
    COMMENT "Listing all source files"
)
