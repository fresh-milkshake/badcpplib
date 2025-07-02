#pragma once

/**
 * @file badcpplib.hpp
 * @brief Main header file for BadCppLib - A comprehensive C++ utility library
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * @copyright MIT License
 *
 * @mainpage BadCppLib Documentation
 *
 * BadCppLib is a comprehensive, header-only C++ utility library designed with modularity in mind.
 * It provides essential tools and data structures for modern C++ development while maintaining
 * minimal compile-time overhead through selective module inclusion.
 *
 * @section features Key Features
 *
 * - **Modular Design**: Include only what you need to minimize binary size
 * - **Modern C++**: Leverages C++14/17/20 features for clean, efficient code
 * - **Header-Only**: No separate compilation required
 * - **Cross-Platform**: Works on Windows, Linux, and macOS
 * - **Zero Dependencies**: Self-contained library (except standard library)
 *
 * @section modules Available Modules
 *
 * | Module | Macro | Description |
 * |--------|-------|-------------|
 * | Core | `BADCPPLIB_ENABLE_CORE` | Basic types and smart pointers (always enabled) |
 * | Result | `BADCPPLIB_ENABLE_RESULT` | Rust-style error handling without exceptions |
 * | String Utils | `BADCPPLIB_ENABLE_STRING` | String manipulation and formatting utilities |
 * | Math Utils | `BADCPPLIB_ENABLE_MATH` | Mathematical functions and random number generation |
 * | Containers | `BADCPPLIB_ENABLE_CONTAINERS` | Custom containers (StaticArray, CircularBuffer) |
 * | File Utils | `BADCPPLIB_ENABLE_FILE_UTILS` | File I/O operations with error handling |
 * | Time Utils | `BADCPPLIB_ENABLE_TIME_UTILS` | Time measurement and benchmarking |
 * | Functional | `BADCPPLIB_ENABLE_FUNCTIONAL` | Functional programming utilities |
 * | Debug | `BADCPPLIB_ENABLE_DEBUG` | Debugging and logging facilities |
 * | Memory | `BADCPPLIB_ENABLE_MEMORY` | Memory management utilities |
 * | Test Framework | `BADCPPLIB_ENABLE_TEST` | Lightweight testing framework |
 *
 * @section usage Quick Start
 *
 * @subsection minimal Minimal Configuration
 * ```cpp
 * #define BADCPPLIB_ENABLE_CORE
 * #define BADCPPLIB_ENABLE_STRING
 * #include "badcpplib/badcpplib.hpp"
 *
 * using namespace badcpplib;
 *
 * int main() {
 *     string text = "  Hello, World!  ";
 *     string trimmed = string_utils::trim(text);
 *     return 0;
 * }
 * ```
 *
 * @subsection full Full Configuration
 * ```cpp
 * #define BADCPPLIB_FULL_BUILD  // Enable all modules
 * #include "badcpplib/badcpplib.hpp"
 *
 * using namespace badcpplib;
 *
 * int main() {
 *     // Use any module functionality
 *     auto result = file_utils::read_file("config.txt");
 *     if (result.is_ok()) {
 *         debug::print("File content:", result.unwrap());
 *     }
 *     return 0;
 * }
 * ```
 *
 * @section build_options Build Configuration Options
 *
 * Use these preprocessor macros to control which modules are included:
 *
 * - `BADCPPLIB_ENABLE_CORE` - Basic types (always included)
 * - `BADCPPLIB_ENABLE_RESULT` - Result type for error handling
 * - `BADCPPLIB_ENABLE_STRING` - String utilities
 * - `BADCPPLIB_ENABLE_MATH` - Mathematical utilities
 * - `BADCPPLIB_ENABLE_CONTAINERS` - Container classes
 * - `BADCPPLIB_ENABLE_FILE_UTILS` - File operations
 * - `BADCPPLIB_ENABLE_FUNCTIONAL` - Functional programming
 * - `BADCPPLIB_ENABLE_DEBUG` - Debug utilities
 * - `BADCPPLIB_ENABLE_TIME_UTILS` - Time measurement
 * - `BADCPPLIB_ENABLE_MEMORY` - Memory management
 * - `BADCPPLIB_ENABLE_TEST` - Testing framework
 *
 * @subsection preset_configs Preset Configurations
 *
 * - `BADCPPLIB_MINIMAL_BUILD` - Core + Result only
 * - `BADCPPLIB_FULL_BUILD` - All modules enabled
 *
 * @section examples Examples
 *
 * See the examples/ directory for comprehensive usage examples:
 * - `minimal_example.cpp` - Basic usage
 * - `full_example.cpp` - All modules demonstration
 * - `simple_example.cpp` - Common use cases
 *
 * @see badcpplib::core
 * @see badcpplib::result
 * @see badcpplib::string_utils
 * @see badcpplib::math_utils
 */

// Library version
#define BADCPPLIB_VERSION_MAJOR 1
#define BADCPPLIB_VERSION_MINOR 0
#define BADCPPLIB_VERSION_PATCH 0

// Preset configurations must be processed BEFORE conditional includes
// Minimal build (only core + result)
#ifdef BADCPPLIB_MINIMAL_BUILD
#ifndef BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_CORE
#endif
#ifndef BADCPPLIB_ENABLE_RESULT
#define BADCPPLIB_ENABLE_RESULT
#endif
#endif

// Full build (all modules)
#ifdef BADCPPLIB_FULL_BUILD
#define BADCPPLIB_ENABLE_RESULT
#define BADCPPLIB_ENABLE_STRING
#define BADCPPLIB_ENABLE_MATH
#define BADCPPLIB_ENABLE_CONTAINERS
#define BADCPPLIB_ENABLE_FILE_UTILS
#define BADCPPLIB_ENABLE_TIME_UTILS
#define BADCPPLIB_ENABLE_FUNCTIONAL
#define BADCPPLIB_ENABLE_DEBUG
#define BADCPPLIB_ENABLE_MEMORY
#define BADCPPLIB_ENABLE_TEST
#endif

// Always include core module
#include "core.hpp"

// Conditional module inclusion
#ifdef BADCPPLIB_ENABLE_RESULT
#include "result.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_STRING
#include "string_utils.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_MATH
#include "math_utils.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_CONTAINERS
#include "containers.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_FILE_UTILS
#include "file_utils.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_TIME_UTILS
#include "time_utils.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_FUNCTIONAL
#include "functional.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_DEBUG
#include "debug.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_MEMORY
#include "memory.hpp"
#endif

#ifdef BADCPPLIB_ENABLE_TEST
#include "test_framework.hpp"
#endif

/**
 * @namespace badcpplib
 * @brief Main namespace for all BadCppLib functionality
 *
 * This namespace contains all modules and utilities provided by BadCppLib.
 * It is designed to be imported with `using namespace badcpplib;` for convenience.
 */
namespace badcpplib
{

    /**
     * @namespace badcpplib::info
     * @brief Library information and build configuration queries
     *
     * This namespace provides compile-time and runtime information about the library
     * version and which modules are enabled in the current build.
     *
     * @example{lineno}
     * ```cpp
     * #include "badcpplib/badcpplib.hpp"
     * using namespace badcpplib;
     *
     * // Check library version
     * if (info::version_major() >= 1) {
     *     std::cout << "Using BadCppLib v" << info::version_string() << std::endl;
     * }
     *
     * // Check available modules
     * if (info::has_string_utils()) {
     *     // Use string utilities
     * }
     * ```
     */
    namespace info
    {

        /**
         * @brief Get the major version number
         * @return Major version as compile-time constant
         * @since{1.0.0}
         * @complexity{O(1)}
         */
        constexpr i32 version_major() { return BADCPPLIB_VERSION_MAJOR; }

        /**
         * @brief Get the minor version number
         * @return Minor version as compile-time constant
         * @since{1.0.0}
         * @complexity{O(1)}
         */
        constexpr i32 version_minor() { return BADCPPLIB_VERSION_MINOR; }

        /**
         * @brief Get the patch version number
         * @return Patch version as compile-time constant
         * @since{1.0.0}
         * @complexity{O(1)}
         */
        constexpr i32 version_patch() { return BADCPPLIB_VERSION_PATCH; }

        /**
         * @brief Get the full version as a formatted string
         * @return Version string in format "major.minor.patch"
         * @since{1.0.0}
         * @complexity{O(1)}
         *
         * @usecase{Library version reporting}
         * Use this function to display or log the library version in applications.
         *
         * @example
         * ```cpp
         * std::cout << "BadCppLib version: " << info::version_string() << std::endl;
         * // Output: BadCppLib version: 1.0.0
         * ```
         */
        string version_string()
        {
            return std::to_string(version_major()) + "." +
                   std::to_string(version_minor()) + "." +
                   std::to_string(version_patch());
        }

        // Information about included modules
        constexpr bool has_core() { return true; } // Always included

        constexpr bool has_result()
        {
#ifdef BADCPPLIB_ENABLE_RESULT
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_string_utils()
        {
#ifdef BADCPPLIB_ENABLE_STRING
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_math_utils()
        {
#ifdef BADCPPLIB_ENABLE_MATH
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_containers()
        {
#ifdef BADCPPLIB_ENABLE_CONTAINERS
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_file_utils()
        {
#ifdef BADCPPLIB_ENABLE_FILE_UTILS
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_time_utils()
        {
#ifdef BADCPPLIB_ENABLE_TIME_UTILS
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_functional()
        {
#ifdef BADCPPLIB_ENABLE_FUNCTIONAL
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_debug()
        {
#ifdef BADCPPLIB_ENABLE_DEBUG
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_memory()
        {
#ifdef BADCPPLIB_ENABLE_MEMORY
            return true;
#else
            return false;
#endif
        }

        constexpr bool has_test_framework()
        {
#ifdef BADCPPLIB_ENABLE_TEST
            return true;
#else
            return false;
#endif
        }

        // Build summary information
        string build_info()
        {
            string info = "BadCppLib v" + version_string() + "\n";
            info += "Enabled modules:\n";

            if (has_core())
                info += "  - core\n";
            if (has_result())
                info += "  - result\n";
            if (has_string_utils())
                info += "  - string_utils\n";
            if (has_math_utils())
                info += "  - math_utils\n";
            if (has_containers())
                info += "  - containers\n";
            if (has_file_utils())
                info += "  - file_utils\n";
            if (has_time_utils())
                info += "  - time_utils\n";
            if (has_functional())
                info += "  - functional\n";
            if (has_debug())
                info += "  - debug\n";
            if (has_memory())
                info += "  - memory\n";
            if (has_test_framework())
                info += "  - test_framework\n";

            return info;
        }

    } // namespace info

} // namespace badcpplib