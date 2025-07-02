#pragma once

/**
 * @file test_framework.hpp
 * @brief Lightweight testing framework for unit testing
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides a simple but powerful testing framework for unit testing
 * with test suites, assertions, and reporting. Enable with
 * `BADCPPLIB_ENABLE_TEST_FRAMEWORK`.
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <sstream>

namespace badcpplib {

/**
 * @namespace badcpplib::testing
 * @brief Lightweight testing framework for unit and integration testing
 * 
 * This namespace provides a complete testing framework with the following features:
 * 
 * **Test Organization:**
 * - TestRunner: Main test execution engine
 * - Test suites for grouping related tests
 * - Individual test cases with timing
 * 
 * **Assertion Macros:**
 * - ASSERT_TRUE/ASSERT_FALSE: Boolean assertions
 * - ASSERT_EQ/ASSERT_NE: Equality assertions
 * - ASSERT_THROWS: Exception testing
 * 
 * **Reporting:**
 * - Color-coded console output
 * - Test timing information
 * - Detailed failure messages with file/line information
 * - Summary statistics
 * 
 * **Easy Integration:**
 * - Simple macro-based test definition
 * - Automatic test registration
 * - Minimal setup required
 * 
 * @section usage Basic Usage
 * 
 * Tests are organized into suites and individual test cases. Each test case
 * can contain multiple assertions. The framework automatically tracks
 * execution time and provides detailed failure information.
 * 
 * @section output Test Output
 * 
 * The framework provides color-coded output with:
 * - Green for passed tests
 * - Red for failed tests
 * - Blue for test suite headers
 * - Execution timing for each test
 * 
 * @section thread_safety Thread Safety
 * 
 * The test framework is not thread-safe. Tests should be run sequentially
 * from a single thread.
 * 
 * @usecase{Unit testing}
 * Perfect for testing individual functions and classes in isolation.
 * 
 * @usecase{Integration testing}
 * Suitable for testing interactions between components.
 * 
 * @usecase{Regression testing}
 * Excellent for automated testing in CI/CD pipelines.
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_TEST_FRAMEWORK
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * int main() {
 *     TEST_SUITE("Math Operations");
 *     
 *     TEST_CASE("Addition") {
 *         ASSERT_EQ(5, 2 + 3);
 *         ASSERT_EQ(10, 7 + 3);
 *     } TEST_END
 *     
 *     TEST_CASE("Division") {
 *         ASSERT_EQ(2, 10 / 5);
 *         ASSERT_THROWS(10 / 0, std::exception);
 *     } TEST_END
 *     
 *     testing::g_test_runner.print_summary();
 *     return testing::g_test_runner.all_passed() ? 0 : 1;
 * }
 * ```
 */
namespace testing {

// Colors for output (headers only)
struct Colors {
    static constexpr const char* GREEN = "\033[32m";
    static constexpr const char* RED = "\033[31m";
    static constexpr const char* YELLOW = "\033[33m";
    static constexpr const char* BLUE = "\033[34m";
    static constexpr const char* RESET = "\033[0m";
    static constexpr const char* BOLD = "\033[1m";
};

// Test result
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double duration_ms;
};

// Test suite information
struct TestSuite {
    std::string name;
    std::vector<TestResult> results;
    int passed = 0;
    int failed = 0;
};

// Main test framework class
class TestRunner {
private:
    std::vector<TestSuite> suites_;
    TestSuite* current_suite_ = nullptr;
    
public:
    // Start new test suite
    void begin_suite(const std::string& name) {
        suites_.emplace_back();
        current_suite_ = &suites_.back();
        current_suite_->name = name;
        std::cout << Colors::BLUE << Colors::BOLD << "\n=== " << name << " ===" << Colors::RESET << std::endl;
    }
    
    // Run test
    void run_test(const std::string& name, std::function<void()> test_func) {
        if (!current_suite_) {
            begin_suite("Default");
        }
        
        std::cout << "  Test: " << name << " ... ";
        
        auto start = std::chrono::high_resolution_clock::now();
        TestResult result;
        result.name = name;
        
        try {
            test_func();
            result.passed = true;
            result.message = "OK";
            current_suite_->passed++;
            
            auto end = std::chrono::high_resolution_clock::now();
            result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            
            std::cout << Colors::GREEN << "PASSED" << Colors::RESET 
                     << " (" << result.duration_ms << "ms)" << std::endl;
                     
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = e.what();
            current_suite_->failed++;
            
            auto end = std::chrono::high_resolution_clock::now();
            result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            
            std::cout << Colors::RED << "FAILED" << Colors::RESET 
                     << " (" << result.duration_ms << "ms)" << std::endl;
            std::cout << "    Error: " << e.what() << std::endl;
        }
        
        current_suite_->results.push_back(result);
    }
    
    // Show overall statistics
    void print_summary() const {
        int total_passed = 0;
        int total_failed = 0;
        double total_time = 0.0;
        
        std::cout << "\n" << Colors::BOLD << "=== TEST SUMMARY ===" << Colors::RESET << std::endl;
        
        for (const auto& suite : suites_) {
            total_passed += suite.passed;
            total_failed += suite.failed;
            
            double suite_time = 0.0;
            for (const auto& result : suite.results) {
                suite_time += result.duration_ms;
            }
            total_time += suite_time;
            
            std::cout << "\n" << Colors::BLUE << suite.name << Colors::RESET << ": ";
            std::cout << Colors::GREEN << suite.passed << " passed" << Colors::RESET << ", ";
            std::cout << Colors::RED << suite.failed << " failed" << Colors::RESET;
            std::cout << " (" << suite_time << "ms)" << std::endl;
        }
        
        std::cout << "\n" << Colors::BOLD << "TOTAL: " << Colors::RESET;
        std::cout << Colors::GREEN << total_passed << " passed" << Colors::RESET << ", ";
        std::cout << Colors::RED << total_failed << " failed" << Colors::RESET;
        std::cout << " (" << total_time << "ms)" << std::endl;
        
        if (total_failed == 0) {
            std::cout << Colors::GREEN << Colors::BOLD << "\n✅ ALL TESTS PASSED!" << Colors::RESET << std::endl;
        } else {
            std::cout << Colors::RED << Colors::BOLD << "\n❌ SOME TESTS FAILED!" << Colors::RESET << std::endl;
        }
    }
    
    // Check if all tests passed
    bool all_passed() const {
        for (const auto& suite : suites_) {
            if (suite.failed > 0) return false;
        }
        return true;
    }
};

// Global test runner instance
extern TestRunner g_test_runner;

// Macros for convenient testing
#define TEST_SUITE(name) badcpplib::testing::g_test_runner.begin_suite(name)

#define TEST_CASE(name) \
    badcpplib::testing::g_test_runner.run_test(name, [&]() {

#define TEST_END });

// Assertion macros
#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " #condition " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(expected, actual) \
    do { \
        auto exp_val = (expected); \
        auto act_val = (actual); \
        if (exp_val != act_val) { \
            std::ostringstream oss; \
            oss << "Expected: " << exp_val << ", but got: " << act_val \
                << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_NE(unexpected, actual) \
    do { \
        auto unexp_val = (unexpected); \
        auto act_val = (actual); \
        if (unexp_val == act_val) { \
            std::ostringstream oss; \
            oss << "Expected not equal to: " << unexp_val << ", but got: " << act_val \
                << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_THROWS(expression, exception_type) \
    do { \
        bool threw = false; \
        try { \
            expression; \
        } catch (const exception_type&) { \
            threw = true; \
        } catch (...) { \
            std::ostringstream oss; \
            oss << "Expected " #exception_type " but got different exception at " \
                << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
        if (!threw) { \
            std::ostringstream oss; \
            oss << "Expected " #exception_type " but no exception was thrown at " \
                << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define RUN_ALL_TESTS() \
    do { \
        badcpplib::testing::g_test_runner.print_summary(); \
        return badcpplib::testing::g_test_runner.all_passed() ? 0 : 1; \
    } while(0)

} // namespace testing
} // namespace badcpplib 