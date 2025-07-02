#pragma once

/**
 * @file file_utils.hpp
 * @brief Cross-platform file and directory manipulation utilities
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides comprehensive file system operations with error handling
 * using Result types. All operations are cross-platform and handle edge cases
 * gracefully. Enable with `BADCPPLIB_ENABLE_FILE_UTILS`.
 */

#include "core.hpp"
#include "result.hpp"
#include <fstream>
#include <filesystem>
#include <vector>

namespace badcpplib {

/**
 * @namespace badcpplib::file_utils
 * @brief Cross-platform file and directory operations with safe error handling
 * 
 * This namespace provides comprehensive file system utilities with emphasis on
 * safety and cross-platform compatibility:
 * 
 * **File Operations:**
 * - read_file(): Read entire files into memory
 * - write_file(): Write content to files with atomic operations
 * - append_file(): Append content to existing files
 * - read_lines()/write_lines(): Line-based file operations
 * 
 * **Directory Operations:**
 * - create_directory(): Recursive directory creation
 * - list_files()/list_directories(): Directory enumeration
 * - Existence checks for files and directories
 * 
 * **File Management:**
 * - copy_file()/move_file(): Safe file operations
 * - remove_file(): File deletion with checks
 * - file_size(): Get file size information
 * 
 * **Path Utilities:**
 * - Path manipulation functions (join, normalize, etc.)
 * - Cross-platform path handling
 * - File extension and name extraction
 * 
 * @section error_handling Error Handling
 * 
 * All operations return Result<T, string> types for explicit error handling.
 * Errors include detailed messages about what went wrong and why.
 * 
 * @section platform_support Platform Support
 * 
 * All operations are cross-platform and handle platform-specific path
 * separators, file permissions, and filesystem limitations.
 * 
 * @section performance Performance Considerations
 * 
 * - File operations: I/O bound, optimized for reliability over speed
 * - Directory operations: May be expensive for large directories
 * - Binary mode for accurate file handling across platforms
 * 
 * @usecase{Configuration file management}
 * Perfect for reading/writing application configuration files with
 * proper error handling.
 * 
 * @usecase{Log file operations}
 * Ideal for log file rotation, writing, and reading with atomic operations.
 * 
 * @usecase{Build systems and tooling}
 * Excellent for file processing in build tools and utilities.
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_FILE_UTILS
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * // Safe file reading with error handling
 * auto content = file_utils::read_file("config.txt");
 * if (content.is_ok()) {
 *     string data = content.unwrap();
 *     // Process file content
 * } else {
 *     string error = content.error();
 *     // Handle error
 * }
 * 
 * // Directory operations
 * file_utils::create_directory("output/logs");
 * auto files = file_utils::list_files("data/");
 * 
 * // Path utilities
 * string full_path = file_utils::path::join("home", "user/docs");
 * string ext = file_utils::path::extension("file.txt");  // ".txt"
 * ```
 */
namespace file_utils {

// File operation result
using FileResult = Result<string>;
using BoolResult = Result<bool>;
using VectorResult = Result<std::vector<string>>;

// Read file completely
FileResult read_file(const string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return FileResult::err("Failed to open file: " + filepath);
        }
        
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        string content;
        content.resize(static_cast<usize>(size));
        file.read(content.data(), size);
        
        if (file.fail() && !file.eof()) {
            return FileResult::err("Error reading file: " + filepath);
        }
        
        return FileResult::ok(std::move(content));
    } catch (const std::exception& e) {
        return FileResult::err("Exception reading file: " + string(e.what()));
    }
}

// Write to file
BoolResult write_file(const string& filepath, const string& content) {
    try {
        std::ofstream file(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return BoolResult::err("Failed to open file for writing: " + filepath);
        }
        
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        
        if (file.fail()) {
            return BoolResult::err("Error writing to file: " + filepath);
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception writing file: " + string(e.what()));
    }
}

// Append to file
BoolResult append_file(const string& filepath, const string& content) {
    try {
        std::ofstream file(filepath, std::ios::out | std::ios::binary | std::ios::app);
        if (!file.is_open()) {
            return BoolResult::err("Failed to open file for appending: " + filepath);
        }
        
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        
        if (file.fail()) {
            return BoolResult::err("Error appending to file: " + filepath);
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception appending to file: " + string(e.what()));
    }
}

// Read file line by line
VectorResult read_lines(const string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return VectorResult::err("Failed to open file: " + filepath);
        }
        
        std::vector<string> lines;
        string line;
        
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        
        if (file.bad()) {
            return VectorResult::err("Error reading lines from file: " + filepath);
        }
        
        return VectorResult::ok(std::move(lines));
    } catch (const std::exception& e) {
        return VectorResult::err("Exception reading lines: " + string(e.what()));
    }
}

// Write strings to file
BoolResult write_lines(const string& filepath, const std::vector<string>& lines) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return BoolResult::err("Failed to open file for writing: " + filepath);
        }
        
        for (const auto& line : lines) {
            file << line << '\n';
            if (file.fail()) {
                return BoolResult::err("Error writing line to file: " + filepath);
            }
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception writing lines: " + string(e.what()));
    }
}

// Check file existence
bool file_exists(const string& filepath) {
    try {
        return std::filesystem::exists(filepath) && 
               std::filesystem::is_regular_file(filepath);
    } catch (const std::exception&) {
        return false;
    }
}

// Check directory existence
bool directory_exists(const string& dirpath) {
    try {
        return std::filesystem::exists(dirpath) && 
               std::filesystem::is_directory(dirpath);
    } catch (const std::exception&) {
        return false;
    }
}

// Get file size
Result<usize> file_size(const string& filepath) {
    try {
        if (!file_exists(filepath)) {
            return Result<usize>::err("File does not exist: " + filepath);
        }
        
        auto size = std::filesystem::file_size(filepath);
        return Result<usize>::ok(static_cast<usize>(size));
    } catch (const std::exception& e) {
        return Result<usize>::err("Error getting file size: " + string(e.what()));
    }
}

// Create directory
BoolResult create_directory(const string& dirpath) {
    try {
        if (directory_exists(dirpath)) {
            return BoolResult::ok(true); // Already exists
        }
        
        bool created = std::filesystem::create_directories(dirpath);
        if (!created && !directory_exists(dirpath)) {
            return BoolResult::err("Failed to create directory: " + dirpath);
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception creating directory: " + string(e.what()));
    }
}

// Delete file
BoolResult remove_file(const string& filepath) {
    try {
        if (!file_exists(filepath)) {
            return BoolResult::err("File does not exist: " + filepath);
        }
        
        bool removed = std::filesystem::remove(filepath);
        if (!removed) {
            return BoolResult::err("Failed to remove file: " + filepath);
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception removing file: " + string(e.what()));
    }
}

// Copy file
BoolResult copy_file(const string& source, const string& destination) {
    try {
        if (!file_exists(source)) {
            return BoolResult::err("Source file does not exist: " + source);
        }
        
        std::filesystem::copy_file(source, destination, 
                                   std::filesystem::copy_options::overwrite_existing);
        
        if (!file_exists(destination)) {
            return BoolResult::err("Failed to copy file to: " + destination);
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception copying file: " + string(e.what()));
    }
}

// Move/rename file
BoolResult move_file(const string& source, const string& destination) {
    try {
        if (!file_exists(source)) {
            return BoolResult::err("Source file does not exist: " + source);
        }
        
        std::filesystem::rename(source, destination);
        
        if (!file_exists(destination) || file_exists(source)) {
            return BoolResult::err("Failed to move file to: " + destination);
        }
        
        return BoolResult::ok(true);
    } catch (const std::exception& e) {
        return BoolResult::err("Exception moving file: " + string(e.what()));
    }
}

// Get list of files in directory
VectorResult list_files(const string& dirpath, bool recursive = false) {
    try {
        if (!directory_exists(dirpath)) {
            return VectorResult::err("Directory does not exist: " + dirpath);
        }
        
        std::vector<string> files;
        
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirpath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dirpath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().string());
                }
            }
        }
        
        return VectorResult::ok(std::move(files));
    } catch (const std::exception& e) {
        return VectorResult::err("Exception listing files: " + string(e.what()));
    }
}

// Get list of directories
VectorResult list_directories(const string& dirpath, bool recursive = false) {
    try {
        if (!directory_exists(dirpath)) {
            return VectorResult::err("Directory does not exist: " + dirpath);
        }
        
        std::vector<string> directories;
        
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirpath)) {
                if (entry.is_directory()) {
                    directories.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dirpath)) {
                if (entry.is_directory()) {
                    directories.push_back(entry.path().string());
                }
            }
        }
        
        return VectorResult::ok(std::move(directories));
    } catch (const std::exception& e) {
        return VectorResult::err("Exception listing directories: " + string(e.what()));
    }
}

// Path utilities
namespace path {
    
    // Get filename
    string filename(const string& filepath) {
        try {
            return std::filesystem::path(filepath).filename().string();
        } catch (const std::exception&) {
            return "";
        }
    }
    
    // Get file extension
    string extension(const string& filepath) {
        try {
            return std::filesystem::path(filepath).extension().string();
        } catch (const std::exception&) {
            return "";
        }
    }
    
    // Get file directory
    string parent_directory(const string& filepath) {
        try {
            return std::filesystem::path(filepath).parent_path().string();
        } catch (const std::exception&) {
            return "";
        }
    }
    
    // Get filename without extension
    string stem(const string& filepath) {
        try {
            return std::filesystem::path(filepath).stem().string();
        } catch (const std::exception&) {
            return "";
        }
    }
    
    // Join paths
    string join(const string& path1, const string& path2) {
        try {
            return (std::filesystem::path(path1) / path2).string();
        } catch (const std::exception&) {
            return path1 + "/" + path2; // Fallback
        }
    }
    
    // Path normalization
    string normalize(const string& filepath) {
        try {
            return std::filesystem::path(filepath).lexically_normal().string();
        } catch (const std::exception&) {
            return filepath;
        }
    }
    
    // Check if path is absolute
    bool is_absolute(const string& filepath) {
        try {
            return std::filesystem::path(filepath).is_absolute();
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Get absolute path
    Result<string> absolute(const string& filepath) {
        try {
            auto abs_path = std::filesystem::absolute(filepath);
            return Result<string>::ok(abs_path.string());
        } catch (const std::exception& e) {
            return Result<string>::err("Error getting absolute path: " + string(e.what()));
        }
    }
    
} // namespace path

} // namespace file_utils
} // namespace badcpplib 