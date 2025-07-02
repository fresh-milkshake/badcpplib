#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_STRING
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("String Utils Module Tests");
    
    TEST_CASE("String splitting")
        string text = "apple,banana,cherry";
        auto parts = string_utils::split(text, ',');
        
        ASSERT_EQ(parts.size(), 3u);
        ASSERT_EQ(parts[0], "apple");
        ASSERT_EQ(parts[1], "banana");
        ASSERT_EQ(parts[2], "cherry");
        
        // Empty string returns empty array
        auto empty_parts = string_utils::split("", ',');
        ASSERT_EQ(empty_parts.size(), 0u);
        
        // String without delimiters
        auto single_parts = string_utils::split("single", ',');
        ASSERT_EQ(single_parts.size(), 1u);
        ASSERT_EQ(single_parts[0], "single");
    TEST_END
    
    TEST_CASE("String joining")
        std::vector<string> parts = {"hello", "world", "test"};
        auto joined = string_utils::join(parts, " ");
        
        ASSERT_EQ(joined, "hello world test");
        
        auto joined_comma = string_utils::join(parts, ", ");
        ASSERT_EQ(joined_comma, "hello, world, test");
        
        // Empty array
        std::vector<string> empty;
        auto empty_joined = string_utils::join(empty, ",");
        ASSERT_EQ(empty_joined, "");
        
        // Single element
        std::vector<string> single = {"alone"};
        auto single_joined = string_utils::join(single, ",");
        ASSERT_EQ(single_joined, "alone");
    TEST_END
    
    TEST_CASE("String trimming")
        ASSERT_EQ(string_utils::trim("  hello  "), "hello");
        ASSERT_EQ(string_utils::trim("\t\ntest\r\f"), "test");
        ASSERT_EQ(string_utils::trim("no_spaces"), "no_spaces");
        ASSERT_EQ(string_utils::trim("   "), "");
        ASSERT_EQ(string_utils::trim(""), "");
        
        ASSERT_EQ(string_utils::trim_left("  hello  "), "hello  ");
        ASSERT_EQ(string_utils::trim_right("  hello  "), "  hello");
    TEST_END
    
    TEST_CASE("Case conversion")
        ASSERT_EQ(string_utils::to_lowercase("HELLO"), "hello");
        ASSERT_EQ(string_utils::to_lowercase("MiXeD"), "mixed");
        ASSERT_EQ(string_utils::to_lowercase("123ABC"), "123abc");
        
        ASSERT_EQ(string_utils::to_uppercase("hello"), "HELLO");
        ASSERT_EQ(string_utils::to_uppercase("MiXeD"), "MIXED");
        ASSERT_EQ(string_utils::to_uppercase("123abc"), "123ABC");
    TEST_END
    
    TEST_CASE("String checks")
        // starts_with
        ASSERT_TRUE(string_utils::starts_with("hello world", "hello"));
        ASSERT_TRUE(string_utils::starts_with("test", "test"));
        ASSERT_FALSE(string_utils::starts_with("hello", "world"));
        ASSERT_FALSE(string_utils::starts_with("hi", "hello"));
        
        // ends_with
        ASSERT_TRUE(string_utils::ends_with("hello world", "world"));
        ASSERT_TRUE(string_utils::ends_with("test", "test"));
        ASSERT_FALSE(string_utils::ends_with("hello", "world"));
        ASSERT_FALSE(string_utils::ends_with("hi", "hello"));
        
        // contains
        ASSERT_TRUE(string_utils::contains("hello world", "lo wo"));
        ASSERT_TRUE(string_utils::contains("test", "es"));
        ASSERT_FALSE(string_utils::contains("hello", "world"));
    TEST_END
    
    TEST_CASE("String replacement")
        // Single replacement
        ASSERT_EQ(string_utils::replace("hello world", "world", "universe"), 
                  "hello universe");
        ASSERT_EQ(string_utils::replace("test", "missing", "replacement"), "test");
        
        // Replace all
        ASSERT_EQ(string_utils::replace_all("hello hello hello", "hello", "hi"), 
                  "hi hi hi");
        ASSERT_EQ(string_utils::replace_all("abcabc", "a", "x"), "xbcxbc");
        ASSERT_EQ(string_utils::replace_all("test", "missing", "replacement"), "test");
    TEST_END
    
    TEST_CASE("String utilities")
        // repeat
        ASSERT_EQ(string_utils::repeat("abc", 3), "abcabcabc");
        ASSERT_EQ(string_utils::repeat("x", 0), "");
        ASSERT_EQ(string_utils::repeat("", 5), "");
        
        // reverse
        ASSERT_EQ(string_utils::reverse("hello"), "olleh");
        ASSERT_EQ(string_utils::reverse("a"), "a");
        ASSERT_EQ(string_utils::reverse(""), "");
        
        // is_empty_or_whitespace
        ASSERT_TRUE(string_utils::is_empty_or_whitespace(""));
        ASSERT_TRUE(string_utils::is_empty_or_whitespace("   "));
        ASSERT_TRUE(string_utils::is_empty_or_whitespace("\t\n\r"));
        ASSERT_FALSE(string_utils::is_empty_or_whitespace("hello"));
        ASSERT_FALSE(string_utils::is_empty_or_whitespace("  a  "));
    TEST_END
    
    TEST_CASE("Number formatting")
        ASSERT_EQ(string_utils::format_number(1234567), "1,234,567");
        ASSERT_EQ(string_utils::format_number(-1234567), "-1,234,567");
        ASSERT_EQ(string_utils::format_number(123), "123");
        ASSERT_EQ(string_utils::format_number(0), "0");
        
        // Custom separator
        ASSERT_EQ(string_utils::format_number(1234567, '.'), "1.234.567");
    TEST_END
    
    TEST_CASE("Bytes formatting")
        ASSERT_EQ(string_utils::format_bytes(512), "512.00 B");
        ASSERT_EQ(string_utils::format_bytes(1024), "1.00 KB");
        ASSERT_EQ(string_utils::format_bytes(1536), "1.50 KB");
        ASSERT_EQ(string_utils::format_bytes(1048576), "1.00 MB");
        ASSERT_EQ(string_utils::format_bytes(1073741824), "1.00 GB");
    TEST_END
    
    RUN_ALL_TESTS();
} 