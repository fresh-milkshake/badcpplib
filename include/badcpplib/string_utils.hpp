#pragma once

/**
 * @file string_utils.hpp
 * @brief String manipulation and formatting utilities
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides comprehensive string manipulation functions including
 * splitting, joining, trimming, case conversion, and formatting utilities.
 * Enable with `BADCPPLIB_ENABLE_STRING`.
 */

#include "core.hpp"
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace badcpplib {

/**
 * @namespace badcpplib::string_utils
 * @brief String manipulation and formatting utilities
 * 
 * This namespace provides a comprehensive set of string manipulation functions
 * commonly needed in C++ applications. All functions are designed to be
 * efficient and safe, working with standard std::string objects.
 * 
 * @section categories Function Categories
 * 
 * - **Splitting and Joining**: Split strings into parts or join collections into strings
 * - **Trimming**: Remove whitespace from beginning, end, or both sides
 * - **Case Conversion**: Convert between uppercase and lowercase
 * - **Searching**: Check for prefixes, suffixes, and substrings
 * - **Replacement**: Replace occurrences of substrings
 * - **Transformation**: Reverse, repeat, and other transformations
 * - **Validation**: Check string properties and contents
 * - **Formatting**: Format numbers and data sizes
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_STRING
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * string text = "  Hello, World!  ";
 * string trimmed = string_utils::trim(text);           // "Hello, World!"
 * auto words = string_utils::split(trimmed, ',');      // ["Hello", " World!"]
 * string upper = string_utils::to_uppercase(trimmed);  // "HELLO, WORLD!"
 * bool has_hello = string_utils::contains(text, "Hello"); // true
 * ```
 */
namespace string_utils {

/**
 * @defgroup string_splitting String Splitting and Joining
 * @brief Functions for splitting strings into parts and joining collections
 * @{
 */

/**
 * @brief Split a string into parts using a delimiter character
 * @param str The string to split
 * @param delimiter The character to use as delimiter
 * @return std::vector<string> Vector of string parts
 * 
 * Splits the input string at each occurrence of the delimiter character.
 * Empty parts are preserved in the result.
 * 
 * @complexity{O(n)} where n is the length of the string
 * @since{1.0.0}
 * 
 * @usecase{CSV parsing}
 * Parse comma-separated values or similar delimited data.
 * 
 * @usecase{Configuration file parsing}
 * Split key-value pairs or configuration entries.
 * 
 * @example
 * ```cpp
 * auto parts = string_utils::split("a,b,c", ',');
 * // Result: ["a", "b", "c"]
 * 
 * auto empty_parts = string_utils::split("a,,c", ',');
 * // Result: ["a", "", "c"]
 * 
 * auto paths = string_utils::split("/usr/local/bin", '/');
 * // Result: ["", "usr", "local", "bin"]
 * ```
 */
std::vector<string> split(const string& str, char delimiter);

/**
 * @brief Join a collection of strings with a delimiter
 * @param strings Vector of strings to join
 * @param delimiter String to insert between each element
 * @return string The joined result
 * 
 * Concatenates all strings in the vector, inserting the delimiter
 * between each pair of adjacent strings.
 * 
 * @complexity{O(n*m)} where n is the number of strings and m is average string length
 * @since{1.0.0}
 * 
 * @usecase{Creating formatted output}
 * Generate comma-separated lists or formatted text output.
 * 
 * @usecase{Path construction}
 * Build file paths from component parts.
 * 
 * @example
 * ```cpp
 * std::vector<string> words = {"Hello", "World", "!"};
 * string sentence = string_utils::join(words, " ");
 * // Result: "Hello World !"
 * 
 * std::vector<string> items = {"apple", "banana", "cherry"};
 * string list = string_utils::join(items, ", ");
 * // Result: "apple, banana, cherry"
 * ```
 */
string join(const std::vector<string>& strings, const string& delimiter);

/** @} */

/**
 * @defgroup string_trimming String Trimming
 * @brief Functions for removing whitespace from strings
 * @{
 */

/**
 * @brief Remove whitespace from both ends of a string
 * @param str The string to trim
 * @return string New string with whitespace removed from both ends
 * 
 * Removes all leading and trailing whitespace characters (spaces, tabs,
 * newlines, etc.) from the string.
 * 
 * @complexity{O(n)} where n is the length of the string
 * @since{1.0.0}
 * 
 * @usecase{User input processing}
 * Clean up user input by removing accidental whitespace.
 * 
 * @usecase{Configuration parsing}
 * Normalize configuration values by removing surrounding whitespace.
 * 
 * @example
 * ```cpp
 * string trimmed = string_utils::trim("  Hello World  ");
 * // Result: "Hello World"
 * 
 * string clean = string_utils::trim("\t\n  Text  \r\n");
 * // Result: "Text"
 * ```
 */
string trim(const string& str);

/**
 * @brief Remove whitespace from the beginning of a string
 * @param str The string to trim
 * @return string New string with leading whitespace removed
 * 
 * @complexity{O(n)} where n is the length of the string
 * @since{1.0.0}
 * 
 * @example
 * ```cpp
 * string result = string_utils::trim_left("  Hello World  ");
 * // Result: "Hello World  "
 * ```
 */
string trim_left(const string& str);

/**
 * @brief Remove whitespace from the end of a string
 * @param str The string to trim
 * @return string New string with trailing whitespace removed
 * 
 * @complexity{O(n)} where n is the length of the string
 * @since{1.0.0}
 * 
 * @example
 * ```cpp
 * string result = string_utils::trim_right("  Hello World  ");
 * // Result: "  Hello World"
 * ```
 */
string trim_right(const string& str);

/** @} */

/**
 * @defgroup string_case Case Conversion
 * @brief Functions for converting string case
 * @{
 */

/**
 * @brief Convert string to lowercase
 * @param str The string to convert
 * @return string New string with all characters in lowercase
 * 
 * @complexity{O(n)} where n is the length of the string
 * @since{1.0.0}
 * 
 * @usecase{Case-insensitive comparison}
 * Normalize strings for case-insensitive operations.
 * 
 * @example
 * ```cpp
 * string lower = string_utils::to_lowercase("Hello World!");
 * // Result: "hello world!"
 * ```
 */
string to_lowercase(const string& str);

/**
 * @brief Convert string to uppercase
 * @param str The string to convert
 * @return string New string with all characters in uppercase
 * 
 * @complexity{O(n)} where n is the length of the string
 * @since{1.0.0}
 * 
 * @usecase{Display formatting}
 * Create headers or emphasized text in uppercase.
 * 
 * @example
 * ```cpp
 * string upper = string_utils::to_uppercase("Hello World!");
 * // Result: "HELLO WORLD!"
 * ```
 */
string to_uppercase(const string& str);

/** @} */

/**
 * @defgroup string_searching String Searching and Testing
 * @brief Functions for searching and testing string contents
 * @{
 */

/**
 * @brief Check if string starts with a prefix
 * @param str The string to check
 * @param prefix The prefix to look for
 * @return bool True if str starts with prefix, false otherwise
 * 
 * @complexity{O(m)} where m is the length of the prefix
 * @since{1.0.0}
 * 
 * @usecase{Protocol detection}
 * Check if URLs start with "http://" or "https://".
 * 
 * @example
 * ```cpp
 * bool is_https = string_utils::starts_with("https://example.com", "https://");
 * // Result: true
 * ```
 */
bool starts_with(const string& str, const string& prefix);

/**
 * @brief Check if string ends with a suffix
 * @param str The string to check
 * @param suffix The suffix to look for
 * @return bool True if str ends with suffix, false otherwise
 * 
 * @complexity{O(m)} where m is the length of the suffix
 * @since{1.0.0}
 * 
 * @usecase{File extension checking}
 * Determine file types by checking extensions.
 * 
 * @example
 * ```cpp
 * bool is_cpp = string_utils::ends_with("main.cpp", ".cpp");
 * // Result: true
 * ```
 */
bool ends_with(const string& str, const string& suffix);

/**
 * @brief Check if string contains a substring
 * @param str The string to search in
 * @param substring The substring to look for
 * @return bool True if substring is found, false otherwise
 * 
 * @complexity{O(n*m)} where n is string length and m is substring length
 * @since{1.0.0}
 * 
 * @usecase{Content filtering}
 * Search for keywords or patterns in text content.
 * 
 * @example
 * ```cpp
 * bool has_world = string_utils::contains("Hello World", "World");
 * // Result: true
 * ```
 */
bool contains(const string& str, const string& substring);

/** @} */

/**
 * @defgroup string_replacement String Replacement
 * @brief Functions for replacing substrings
 * @{
 */

/**
 * @brief Replace first occurrence of a substring
 * @param str The source string
 * @param from The substring to replace
 * @param to The replacement string
 * @return string New string with first occurrence replaced
 * 
 * @complexity{O(n)} where n is the length of the result string
 * @since{1.0.0}
 * 
 * @example
 * ```cpp
 * string result = string_utils::replace("Hello World World", "World", "C++");
 * // Result: "Hello C++ World"
 * ```
 */
string replace(const string& str, const string& from, const string& to);

/**
 * @brief Replace all occurrences of a substring
 * @param str The source string
 * @param from The substring to replace
 * @param to The replacement string
 * @return string New string with all occurrences replaced
 * 
 * @complexity{O(n*m)} where n is result length and m is number of replacements
 * @since{1.0.0}
 * 
 * @usecase{Template processing}
 * Replace placeholder variables in template strings.
 * 
 * @example
 * ```cpp
 * string result = string_utils::replace_all("Hello World World", "World", "C++");
 * // Result: "Hello C++ C++"
 * ```
 */
string replace_all(const string& str, const string& from, const string& to);

/** @} */

/**
 * @defgroup string_transformation String Transformation
 * @brief Functions for transforming strings
 * @{
 */

/**
 * @brief Repeat a string multiple times
 * @param str The string to repeat
 * @param count Number of times to repeat
 * @return string New string containing the repeated content
 * 
 * @complexity{O(n*count)} where n is the string length
 * @since{1.0.0}
 * 
 * @usecase{Text formatting}
 * Create padding, separators, or decorative elements.
 * 
 * @example
 * ```cpp
 * string stars = string_utils::repeat("*", 10);
 * // Result: "**********"
 * 
 * string padding = string_utils::repeat(" ", 4);
 * // Result: "    "
 * ```
 */
string repeat(const string& str, usize count);

/**
 * @brief Reverse the characters in a string
 * @param str The string to reverse
 * @return string New string with characters in reverse order
 * 
 * @complexity{O(n)} where n is the string length
 * @since{1.0.0}
 * 
 * @usecase{Algorithm implementation}
 * Implement palindrome checking or string reversal algorithms.
 * 
 * @example
 * ```cpp
 * string reversed = string_utils::reverse("Hello");
 * // Result: "olleH"
 * ```
 */
string reverse(const string& str);

/** @} */

/**
 * @defgroup string_validation String Validation
 * @brief Functions for validating string properties
 * @{
 */

/**
 * @brief Check if string is empty or contains only whitespace
 * @param str The string to check
 * @return bool True if string is empty or whitespace-only, false otherwise
 * 
 * @complexity{O(n)} where n is the string length
 * @since{1.0.0}
 * 
 * @usecase{Input validation}
 * Validate that user input contains meaningful content.
 * 
 * @example
 * ```cpp
 * bool empty1 = string_utils::is_empty_or_whitespace("");        // true
 * bool empty2 = string_utils::is_empty_or_whitespace("   ");     // true
 * bool empty3 = string_utils::is_empty_or_whitespace(" a ");     // false
 * ```
 */
bool is_empty_or_whitespace(const string& str);

/** @} */

/**
 * @defgroup string_formatting String Formatting
 * @brief Functions for formatting data as strings
 * @{
 */

/**
 * @brief Format a number with thousands separators
 * @param number The number to format
 * @param separator Character to use as thousands separator (default: comma)
 * @return string Formatted number string
 * 
 * @complexity{O(log₁₀(number))} 
 * @since{1.0.0}
 * 
 * @usecase{User interface display}
 * Display large numbers in human-readable format.
 * 
 * @example
 * ```cpp
 * string formatted1 = string_utils::format_number(1234567);
 * // Result: "1,234,567"
 * 
 * string formatted2 = string_utils::format_number(1234567, '.');
 * // Result: "1.234.567"
 * ```
 */
string format_number(i64 number, char separator = ',');

/**
 * @brief Format byte count in human-readable units
 * @param bytes Number of bytes to format
 * @return string Formatted byte string with appropriate unit (B, KB, MB, GB, TB)
 * 
 * @complexity{O(1)}
 * @since{1.0.0}
 * 
 * @usecase{File size display}
 * Show file sizes or memory usage in user-friendly format.
 * 
 * @example
 * ```cpp
 * string size1 = string_utils::format_bytes(1024);      // "1.0 KB"
 * string size2 = string_utils::format_bytes(1048576);   // "1.0 MB" 
 * string size3 = string_utils::format_bytes(500);       // "500 B"
 * ```
 */
string format_bytes(u64 bytes);

/** @} */

} // namespace string_utils
} // namespace badcpplib 