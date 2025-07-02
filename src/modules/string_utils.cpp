#include "badcpplib/string_utils.hpp"
#include <algorithm>
#include <sstream>

namespace badcpplib {
namespace string_utils {

std::vector<string> split(const string& str, char delimiter) {
    std::vector<string> result;
    
    if (str.empty()) {
        return result; // Empty vector for empty string
    }
    
    std::stringstream ss(str);
    string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

string join(const std::vector<string>& strings, const string& delimiter) {
    if (strings.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << strings[0];
    
    for (usize i = 1; i < strings.size(); ++i) {
        oss << delimiter << strings[i];
    }
    
    return oss.str();
}

string trim(const string& str) {
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos) {
        return "";
    }
    
    auto end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

string trim_left(const string& str) {
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos) {
        return "";
    }
    return str.substr(start);
}

string trim_right(const string& str) {
    auto end = str.find_last_not_of(" \t\n\r\f\v");
    if (end == string::npos) {
        return "";
    }
    return str.substr(0, end + 1);
}

string to_lowercase(const string& str) {
    string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

string to_uppercase(const string& str) {
    string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool starts_with(const string& str, const string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

bool ends_with(const string& str, const string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

bool contains(const string& str, const string& substring) {
    return str.find(substring) != string::npos;
}

string replace(const string& str, const string& from, const string& to) {
    if (from.empty()) {
        return str;
    }
    
    auto pos = str.find(from);
    if (pos == string::npos) {
        return str;
    }
    
    string result = str;
    result.replace(pos, from.length(), to);
    return result;
}

string replace_all(const string& str, const string& from, const string& to) {
    if (from.empty()) {
        return str;
    }
    
    string result = str;
    usize pos = 0;
    
    while ((pos = result.find(from, pos)) != string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

string repeat(const string& str, usize count) {
    if (count == 0) {
        return "";
    }
    
    string result;
    result.reserve(str.length() * count);
    
    for (usize i = 0; i < count; ++i) {
        result += str;
    }
    
    return result;
}

string reverse(const string& str) {
    string result = str;
    std::reverse(result.begin(), result.end());
    return result;
}

bool is_empty_or_whitespace(const string& str) {
    return str.find_first_not_of(" \t\n\r\f\v") == string::npos;
}

string format_number(i64 number, char separator) {
    string str = std::to_string(number);
    
    if (str.length() <= 3) {
        return str;
    }
    
    string result;
    i32 count = 0;
    
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (count > 0 && count % 3 == 0 && *it != '-') {
            result = separator + result;
        }
        result = *it + result;
        ++count;
    }
    
    return result;
}

string format_bytes(u64 bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    const usize num_units = sizeof(units) / sizeof(units[0]);
    
    f64 size = static_cast<f64>(bytes);
    usize unit_index = 0;
    
    while (size >= 1024.0 && unit_index < num_units - 1) {
        size /= 1024.0;
        ++unit_index;
    }
    
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed << size << " " << units[unit_index];
    
    return oss.str();
}

} // namespace string_utils
} // namespace badcpplib 