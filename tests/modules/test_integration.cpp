#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_RESULT
#define BADCPPLIB_ENABLE_STRING
#define BADCPPLIB_ENABLE_MATH
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Integration Tests");
    
    TEST_CASE("Result with string operations")
        // Test integration of Result with string_utils
        auto process_text = [](const string& text) -> Result<string> {
            if (text.empty()) {
                return Result<string>::err("Empty text provided");
            }
            
            auto parts = string_utils::split(text, ',');
            auto joined = string_utils::join(parts, " | ");
            auto upper = string_utils::to_uppercase(joined);
            
            return Result<string>::ok(upper);
        };
        
        auto success_result = process_text("hello,world,test");
        ASSERT_TRUE(success_result.is_ok());
        ASSERT_EQ(success_result.unwrap(), "HELLO | WORLD | TEST");
        
        auto error_result = process_text("");
        ASSERT_TRUE(error_result.is_err());
        ASSERT_EQ(error_result.error(), "Empty text provided");
    TEST_END
    
    TEST_CASE("Result with math operations")
        // Test integration of Result with math_utils
        auto safe_divide = [](f64 a, f64 b) -> Result<f64> {
            if (b == 0.0) {
                return Result<f64>::err("Division by zero");
            }
            return Result<f64>::ok(a / b);
        };
        
        auto valid_division = safe_divide(10.0, 2.0);
        ASSERT_TRUE(valid_division.is_ok());
        ASSERT_EQ(valid_division.unwrap(), 5.0);
        
        auto invalid_division = safe_divide(10.0, 0.0);
        ASSERT_TRUE(invalid_division.is_err());
        ASSERT_EQ(invalid_division.error(), "Division by zero");
        
        // Integration with clamp
        auto clamped_result = valid_division.map([](f64 val) {
            return math_utils::clamp(val, 0.0, 10.0);
        });
        
        ASSERT_TRUE(clamped_result.is_ok());
        ASSERT_EQ(clamped_result.unwrap(), 5.0);
    TEST_END
    
    TEST_CASE("String formatting with math")
        // Integration of string_utils and math_utils
        math_utils::Random rng(42);
        
        std::vector<string> formatted_numbers;
        for (int i = 0; i < 5; ++i) {
            i32 num = rng.next_int(1000, 999999);
            string formatted = string_utils::format_number(num);
            formatted_numbers.push_back(formatted);
        }
        
        string result = string_utils::join(formatted_numbers, " ; ");
        ASSERT_FALSE(result.empty());
        ASSERT_TRUE(string_utils::contains(result, ","));
        ASSERT_TRUE(string_utils::contains(result, " ; "));
    TEST_END
    
    TEST_CASE("Complex data processing pipeline")
        // Complex test of all modules together
        string input = "3.14159,2.71828,1.41421";
        auto parts = string_utils::split(input, ',');
        
        f64 sum = 0.0;
        for (const auto& part : parts) {
            try {
                f64 value = std::stod(part);
                sum += value;
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        // Use math_utils for rounding
        f64 rounded = std::round(sum * 100.0) / 100.0;
        ASSERT_TRUE(rounded > 7.0 && rounded < 8.0); // Approximately 7.27
        
        // Format result as string
        string result_str = "Sum: " + std::to_string(rounded);
        result_str = string_utils::trim(result_str);
        
        ASSERT_TRUE(string_utils::starts_with(result_str, "Sum:"));
        ASSERT_TRUE(string_utils::contains(result_str, std::to_string(rounded)));
    TEST_END
    
    TEST_CASE("Error handling across modules")
        // Error handling between modules
        auto process_numbers = [](const string& input) -> Result<f64> {
            if (string_utils::is_empty_or_whitespace(input)) {
                return Result<f64>::err("Empty input");
            }
            
            auto parts = string_utils::split(input, ',');
            if (parts.empty()) {
                return Result<f64>::err("No valid parts found");
            }
            
            f64 sum = 0.0;
            for (const auto& part : parts) {
                try {
                    f64 value = std::stod(string_utils::trim(part));
                    sum += value;
                } catch (...) {
                    return Result<f64>::err("Invalid number format");
                }
            }
            
            return Result<f64>::ok(sum);
        };
        
        auto valid_result = process_numbers("1.5, 2.5, 3.0");
        ASSERT_TRUE(valid_result.is_ok());
        ASSERT_EQ(valid_result.unwrap(), 7.0);
        
        auto empty_result = process_numbers("   ");
        ASSERT_TRUE(empty_result.is_err());
        ASSERT_EQ(empty_result.error(), "Empty input");
        
        auto invalid_result = process_numbers("1.5, abc, 3.0");
        ASSERT_TRUE(invalid_result.is_err());
        ASSERT_EQ(invalid_result.error(), "Invalid number format");
    TEST_END
    
    RUN_ALL_TESTS();
} 