#pragma once

/**
 * @file debug.hpp
 * @brief Comprehensive debugging and profiling utilities
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides debugging utilities including logging, profiling, memory tracking,
 * and debug printing facilities. Enable with `BADCPPLIB_ENABLE_DEBUG`.
 */

#include "core.hpp"
#include "time_utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <iomanip>
#include <map>
#include <atomic>
#include <chrono>

namespace badcpplib {

/**
 * @namespace badcpplib::debug
 * @brief Debugging, logging, and profiling utilities
 * 
 * This namespace provides comprehensive debugging tools for development and production:
 * 
 * **Logging System:**
 * - Logger: Multi-level logging with file and console output
 * - LogLevel: Configurable log levels from TRACE to CRITICAL
 * - Color-coded console output with thread-safe operation
 * 
 * **Performance Profiling:**
 * - Profiler: Performance measurement and reporting
 * - ScopedProfiler: RAII-based automatic profiling
 * - Time-based profiling with statistics
 * 
 * **Memory Debugging:**
 * - AllocationTracker: Track memory allocations and leaks
 * - Memory usage statistics and leak detection
 * 
 * **Debug Utilities:**
 * - Debug printing for containers and custom types
 * - Hex dump functionality for binary data analysis
 * - Memory visualization tools
 * 
 * @section logging Logging System
 * 
 * The logging system supports multiple output targets (console, file), configurable
 * log levels, and thread-safe operation with formatted output including timestamps,
 * thread IDs, and source location information.
 * 
 * @section profiling Performance Profiling
 * 
 * The profiling system provides both manual and automatic (RAII) profiling with
 * statistical analysis including min/max/average execution times.
 * 
 * @section thread_safety Thread Safety
 * 
 * All debugging utilities are thread-safe and can be used safely in multi-threaded
 * applications without external synchronization.
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_DEBUG
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * // Logging system
 * auto& logger = debug::get_global_logger();
 * logger.set_min_level(debug::LogLevel::DEBUG);
 * logger.info("Application started");
 * logger.error("Something went wrong");
 * 
 * // Performance profiling
 * {
 *     debug::ScopedProfiler prof("expensive_operation");
 *     // ... expensive code here ...
 * }  // Automatically reports timing
 * 
 * // Memory tracking
 * debug::memory::AllocationTracker tracker;
 * // ... allocate/deallocate memory ...
 * tracker.print_statistics();
 * ```
 */
namespace debug {

// Logging levels
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
};

// Convert level to string
string log_level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

// Output operator for LogLevel
std::ostream& operator<<(std::ostream& os, LogLevel level) {
    return os << log_level_to_string(level);
}

// Colors for console output
namespace colors {
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    const string BOLD = "\033[1m";
    const string DIM = "\033[2m";
}

// Get color for log level
string get_log_color(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return colors::DIM + colors::WHITE;
        case LogLevel::DEBUG: return colors::CYAN;
        case LogLevel::INFO: return colors::GREEN;
        case LogLevel::WARNING: return colors::YELLOW;
        case LogLevel::ERROR: return colors::RED;
        case LogLevel::CRITICAL: return colors::BOLD + colors::RED;
        default: return colors::RESET;
    }
}

// Simple logger
class Logger {
private:
    LogLevel min_level_;
    bool use_colors_;
    bool log_to_file_;
    string log_file_path_;
    std::ofstream log_file_;
    std::mutex log_mutex_;
    
    string format_message(LogLevel level, const string& message, 
                         const string& file = "", i32 line = 0) {
        std::stringstream ss;
        
        // Time
        ss << "[" << time_utils::format::current_time("%H:%M:%S") << "] ";
        
        // Level with colors (console only)
        if (use_colors_ && !log_to_file_) {
            ss << get_log_color(level) << "[" << log_level_to_string(level) << "]" 
               << colors::RESET << " ";
        } else {
            ss << "[" << log_level_to_string(level) << "] ";
        }
        
        // File and line (if specified)
        if (!file.empty() && line > 0) {
            ss << "[" << file << ":" << line << "] ";
        }
        
        // Thread (thread ID)
        ss << "[T:" << std::this_thread::get_id() << "] ";
        
        // Message
        ss << message;
        
        return ss.str();
    }
    
public:
    Logger(LogLevel min_level = LogLevel::INFO, bool use_colors = true) 
        : min_level_(min_level), use_colors_(use_colors), log_to_file_(false) {}
    
    void set_min_level(LogLevel level) { min_level_ = level; }
    LogLevel get_min_level() const { return min_level_; }
    
    void enable_colors(bool enable) { use_colors_ = enable; }
    bool colors_enabled() const { return use_colors_; }
    
    bool enable_file_logging(const string& file_path) {
        std::lock_guard<std::mutex> lock(log_mutex_);
        
        if (log_file_.is_open()) {
            log_file_.close();
        }
        
        log_file_.open(file_path, std::ios::app);
        if (log_file_.is_open()) {
            log_to_file_ = true;
            log_file_path_ = file_path;
            return true;
        }
        
        return false;
    }
    
    void disable_file_logging() {
        std::lock_guard<std::mutex> lock(log_mutex_);
        
        if (log_file_.is_open()) {
            log_file_.close();
        }
        log_to_file_ = false;
    }
    
    void log(LogLevel level, const string& message, 
             const string& file = "", i32 line = 0) {
        if (level < min_level_) return;
        
        string formatted = format_message(level, message, file, line);
        
        std::lock_guard<std::mutex> lock(log_mutex_);
        
        // Console output
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
        
        // File output
        if (log_to_file_ && log_file_.is_open()) {
            // Write to file without colors
            string file_message = format_message(level, message, file, line);
            log_file_ << file_message << std::endl;
            log_file_.flush();
        }
    }
    
    void trace(const string& message, const string& file = "", i32 line = 0) {
        log(LogLevel::TRACE, message, file, line);
    }
    
    void debug(const string& message, const string& file = "", i32 line = 0) {
        log(LogLevel::DEBUG, message, file, line);
    }
    
    void info(const string& message, const string& file = "", i32 line = 0) {
        log(LogLevel::INFO, message, file, line);
    }
    
    void warning(const string& message, const string& file = "", i32 line = 0) {
        log(LogLevel::WARNING, message, file, line);
    }
    
    void error(const string& message, const string& file = "", i32 line = 0) {
        log(LogLevel::ERROR, message, file, line);
    }
    
    void critical(const string& message, const string& file = "", i32 line = 0) {
        log(LogLevel::CRITICAL, message, file, line);
    }
};

// Global logger
Logger& get_global_logger() {
    static Logger global_logger;
    return global_logger;
}

// Macros for convenient logging
#define LOG_TRACE(msg) badcpplib::debug::get_global_logger().trace(msg, __FILE__, __LINE__)
#define LOG_DEBUG(msg) badcpplib::debug::get_global_logger().debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) badcpplib::debug::get_global_logger().info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) badcpplib::debug::get_global_logger().warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) badcpplib::debug::get_global_logger().error(msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg) badcpplib::debug::get_global_logger().critical(msg, __FILE__, __LINE__)

// Assertion macros
#ifdef NDEBUG
    #define ASSERT(condition) ((void)0)
    #define ASSERT_MSG(condition, msg) ((void)0)
#else
    #define ASSERT(condition) \
        do { \
            if (!(condition)) { \
                badcpplib::debug::get_global_logger().critical( \
                    "Assertion failed: " #condition, __FILE__, __LINE__); \
                std::abort(); \
            } \
        } while(0)
    
    #define ASSERT_MSG(condition, msg) \
        do { \
            if (!(condition)) { \
                badcpplib::debug::get_global_logger().critical( \
                    "Assertion failed: " #condition " - " + string(msg), __FILE__, __LINE__); \
                std::abort(); \
            } \
        } while(0)
#endif

// Additional assertion macros
#define DEBUG_ASSERT_EQ(a, b) ASSERT_MSG((a) == (b), "Expected " #a " == " #b)
#define DEBUG_ASSERT_NE(a, b) ASSERT_MSG((a) != (b), "Expected " #a " != " #b)
#define DEBUG_ASSERT_LT(a, b) ASSERT_MSG((a) < (b), "Expected " #a " < " #b)
#define DEBUG_ASSERT_LE(a, b) ASSERT_MSG((a) <= (b), "Expected " #a " <= " #b)
#define DEBUG_ASSERT_GT(a, b) ASSERT_MSG((a) > (b), "Expected " #a " > " #b)
#define DEBUG_ASSERT_GE(a, b) ASSERT_MSG((a) >= (b), "Expected " #a " >= " #b)

#define ASSERT_NULL(ptr) ASSERT_MSG((ptr) == nullptr, #ptr " should be null")
#define ASSERT_NOT_NULL(ptr) ASSERT_MSG((ptr) != nullptr, #ptr " should not be null")

// Performance profiler
class Profiler {
private:
    struct ProfileData {
        usize call_count = 0;
        time_utils::Duration total_time{};
        time_utils::Duration min_time{time_utils::Duration::max()};
        time_utils::Duration max_time{};
    };
    
    std::map<string, ProfileData> profile_data_;
    mutable std::mutex profile_mutex_;
    
public:
    void begin_profile(const string& name) {
        // Placeholder - used by macros to start profiling
        (void)name;
    }
    
    void end_profile(const string& name, time_utils::Duration duration) {
        std::lock_guard<std::mutex> lock(profile_mutex_);
        
        auto& data = profile_data_[name];
        data.call_count++;
        data.total_time = data.total_time + duration;
        
        if (duration < data.min_time) {
            data.min_time = duration;
        }
        if (duration > data.max_time) {
            data.max_time = duration;
        }
    }
    
    void print_report() const {
        std::lock_guard<std::mutex> lock(profile_mutex_);
        
        std::cout << "\n=== Performance Report ===\n";
        for (const auto& [name, data] : profile_data_) {
            auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(data.total_time);
            auto min_ms = std::chrono::duration_cast<std::chrono::milliseconds>(data.min_time);
            auto max_ms = std::chrono::duration_cast<std::chrono::milliseconds>(data.max_time);
            f64 avg_ms = static_cast<f64>(total_ms.count()) / data.call_count;
            
            std::cout << name << ":\n";
            std::cout << "  Calls: " << data.call_count << "\n";
            std::cout << "  Total: " << std::fixed << std::setprecision(3) << total_ms.count() << " ms\n";
            std::cout << "  Average: " << avg_ms << " ms\n";
            std::cout << "  Min: " << min_ms.count() << " ms\n";
            std::cout << "  Max: " << max_ms.count() << " ms\n\n";
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(profile_mutex_);
        profile_data_.clear();
    }
};

// Global profiler
Profiler& get_global_profiler() {
    static Profiler global_profiler;
    return global_profiler;
}

// RAII profiler for automatic time measurement
class ScopedProfiler {
private:
    string name_;
    time_utils::Timer timer_;
    
public:
    explicit ScopedProfiler(const string& name) : name_(name) {
        timer_.start();
    }
    
    ~ScopedProfiler() {
        auto elapsed = timer_.stop();
        get_global_profiler().end_profile(name_, elapsed);
    }
};

// Macro for function profiling
#define PROFILE_FUNCTION() badcpplib::debug::ScopedProfiler _prof(__FUNCTION__)
#define PROFILE_SCOPE(name) badcpplib::debug::ScopedProfiler _prof(name)

// Memory debugging utilities
namespace memory {
    
    // Simple allocation counter
    class AllocationTracker {
    private:
        std::atomic<usize> allocation_count_{0};
        std::atomic<usize> deallocation_count_{0};
        std::atomic<usize> total_allocated_bytes_{0};
        std::atomic<usize> current_allocated_bytes_{0};
        
        mutable std::mutex allocations_mutex_;
        std::map<void*, usize> allocations_;
        
    public:
        void track_allocation(void* ptr, usize size) {
            std::lock_guard<std::mutex> lock(allocations_mutex_);
            allocations_[ptr] = size;
            allocation_count_++;
            total_allocated_bytes_ += size;
            current_allocated_bytes_ += size;
        }
        
        void track_deallocation(void* ptr) {
            std::lock_guard<std::mutex> lock(allocations_mutex_);
            auto it = allocations_.find(ptr);
            if (it != allocations_.end()) {
                usize size = it->second;
                current_allocated_bytes_ -= size;
                deallocation_count_++;
                allocations_.erase(it);
            }
        }
        
        usize get_allocation_count() const { return allocation_count_.load(); }
        usize get_deallocation_count() const { return deallocation_count_.load(); }
        usize get_total_allocated_bytes() const { return total_allocated_bytes_.load(); }
        usize get_current_allocated_bytes() const { return current_allocated_bytes_.load(); }
        
        void print_statistics() const {
            std::lock_guard<std::mutex> lock(allocations_mutex_);
            
            std::cout << "\n=== Memory Statistics ===\n";
            std::cout << "Allocations: " << allocation_count_.load() << "\n";
            std::cout << "Deallocations: " << deallocation_count_.load() << "\n";
            std::cout << "Leaked allocations: " << (allocation_count_.load() - deallocation_count_.load()) << "\n";
            std::cout << "Total allocated: " << total_allocated_bytes_.load() << " bytes\n";
            std::cout << "Currently allocated: " << current_allocated_bytes_.load() << " bytes\n";
            std::cout << "Active allocations: " << allocations_.size() << "\n";
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(allocations_mutex_);
            allocation_count_ = 0;
            deallocation_count_ = 0;
            total_allocated_bytes_ = 0;
            current_allocated_bytes_ = 0;
            allocations_.clear();
        }
    };
    
    // Global allocation tracker
    AllocationTracker& get_global_tracker() {
        static AllocationTracker global_tracker;
        return global_tracker;
    }
    
} // namespace memory

// Utilities for outputting debug information
template<typename T>
string debug_print(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Specialization for containers
template<typename Container>
string debug_print_container(const Container& container, const string& name = "container") {
    std::stringstream ss;
    ss << name << " [size=" << container.size() << "]: {";
    
    bool first = true;
    for (const auto& item : container) {
        if (!first) ss << ", ";
        ss << debug_print(item);
        first = false;
    }
    
    ss << "}";
    return ss.str();
}

// Macros for debug output
#define DEBUG_PRINT(var) LOG_DEBUG(#var " = " + badcpplib::debug::debug_print(var))
#define DEBUG_PRINT_CONTAINER(container) LOG_DEBUG(badcpplib::debug::debug_print_container(container, #container))

// Utility for hex dump
string hex_dump(const void* data, usize size, usize bytes_per_line = 16) {
    const u8* bytes = static_cast<const u8*>(data);
    std::stringstream ss;
    
    for (usize i = 0; i < size; i += bytes_per_line) {
        // Offset
        ss << std::hex << std::setfill('0') << std::setw(8) << i << ": ";
        
        // Hex values
        for (usize j = 0; j < bytes_per_line; ++j) {
            if (i + j < size) {
                ss << std::hex << std::setfill('0') << std::setw(2) 
                   << static_cast<i32>(bytes[i + j]) << " ";
            } else {
                ss << "   ";
            }
        }
        
        ss << " ";
        
        // ASCII representation
        for (usize j = 0; j < bytes_per_line && i + j < size; ++j) {
            u8 byte = bytes[i + j];
            if (byte >= 32 && byte <= 126) {
                ss << static_cast<char>(byte);
            } else {
                ss << ".";
            }
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

#define HEX_DUMP(data, size) LOG_DEBUG("Hex dump of " #data ":\n" + badcpplib::debug::hex_dump(data, size))

} // namespace debug
} // namespace badcpplib 