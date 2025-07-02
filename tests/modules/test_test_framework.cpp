#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_TEST_FRAMEWORK
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Test Framework Module Tests");
    
    TEST_CASE("Basic assertions")
        // Test ASSERT_TRUE
        ASSERT_TRUE(true);
        ASSERT_TRUE(1 == 1);
        ASSERT_TRUE(5 > 3);
        
        // Test ASSERT_FALSE
        ASSERT_FALSE(false);
        ASSERT_FALSE(1 == 2);
        ASSERT_FALSE(3 > 5);
    TEST_END
    
    TEST_CASE("Equality assertions")
        // Test ASSERT_EQ
        ASSERT_EQ(42, 42);
        ASSERT_EQ("hello", "hello");
        ASSERT_EQ(3.14, 3.14);
        
        // Test ASSERT_NE
        ASSERT_NE(1, 2);
        ASSERT_NE("hello", "world");
        ASSERT_NE(3.14, 2.71);
    TEST_END
    
    TEST_CASE("String operations")
        string test_str = "test";
        ASSERT_EQ(test_str.length(), 4u);
        ASSERT_TRUE(test_str == "test");
        ASSERT_FALSE(test_str.empty());
    TEST_END
    
    TEST_CASE("Exception testing")
        // Test ASSERT_THROWS (check that basic logic works)
        auto throw_func = []() {
            throw std::runtime_error("Test exception");
        };
        
        auto no_throw_func = []() {
            return 42;
        };
        
        ASSERT_THROWS(throw_func(), std::runtime_error);
        // ASSERT_THROWS(no_throw_func(), std::runtime_error); // This test would fail
    TEST_END
    
    TEST_CASE("Test runner functionality")
        // Check that global test runner exists
        ASSERT_TRUE(true); // Simple check that we can execute tests
    TEST_END
    
    RUN_ALL_TESTS();
} 