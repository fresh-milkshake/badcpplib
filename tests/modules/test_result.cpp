#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_RESULT
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Result Module Tests");
    
    TEST_CASE("Result creation")
        auto ok_result = Result<i32>::ok(42);
        auto err_result = Result<i32>::err("Error message");
        
        ASSERT_TRUE(ok_result.is_ok());
        ASSERT_FALSE(ok_result.is_err());
        
        ASSERT_FALSE(err_result.is_ok());
        ASSERT_TRUE(err_result.is_err());
    TEST_END
    
    TEST_CASE("Result unwrapping")
        auto ok_result = Result<string>::ok("Hello");
        
        ASSERT_EQ(ok_result.unwrap(), "Hello");
        ASSERT_EQ(ok_result.expect("Should not fail"), "Hello");
        
        auto err_result = Result<string>::err("Failed");
        ASSERT_THROWS(err_result.unwrap(), std::runtime_error);
        ASSERT_THROWS(err_result.expect("Expected failure"), std::runtime_error);
    TEST_END
    
    TEST_CASE("Error handling")
        auto err_result = Result<i32>::err("Test error");
        
        ASSERT_EQ(err_result.error(), "Test error");
        
        auto ok_result = Result<i32>::ok(100);
        ASSERT_THROWS(ok_result.error(), std::runtime_error);
    TEST_END
    
    TEST_CASE("Default values")
        auto ok_result = Result<i32>::ok(42);
        auto err_result = Result<i32>::err("Error");
        
        ASSERT_EQ(ok_result.unwrap_or(0), 42);
        ASSERT_EQ(err_result.unwrap_or(0), 0);
        ASSERT_EQ(err_result.unwrap_or(999), 999);
    TEST_END
    
    TEST_CASE("Result mapping")
        auto ok_result = Result<i32>::ok(5);
        
        auto mapped = ok_result.map([](i32 x) { return x * 2; });
        ASSERT_TRUE(mapped.is_ok());
        ASSERT_EQ(mapped.unwrap(), 10);
        
        auto err_result = Result<i32>::err("Error");
        auto mapped_err = err_result.map([](i32 x) { return x * 2; });
        ASSERT_TRUE(mapped_err.is_err());
        ASSERT_EQ(mapped_err.error(), "Error");
    TEST_END
    
    TEST_CASE("Error mapping")
        auto err_result = Result<i32>::err("original error");
        
        auto mapped_err = err_result.map_err([](const string& e) {
            return "mapped: " + e;
        });
        
        ASSERT_TRUE(mapped_err.is_err());
        ASSERT_EQ(mapped_err.error(), "mapped: original error");
        
        auto ok_result = Result<i32>::ok(42);
        auto mapped_ok = ok_result.map_err([](const string& e) {
            return "should not happen: " + e;
        });
        
        ASSERT_TRUE(mapped_ok.is_ok());
        ASSERT_EQ(mapped_ok.unwrap(), 42);
    TEST_END
    
    TEST_CASE("Result chaining")
        auto ok_result = Result<i32>::ok(10);
        
        auto chained = ok_result.and_then([](i32 x) {
            if (x > 5) {
                return Result<string>::ok("big number");
            } else {
                return Result<string>::err("small number");
            }
        });
        
        ASSERT_TRUE(chained.is_ok());
        ASSERT_EQ(chained.unwrap(), "big number");
        
        auto err_result = Result<i32>::err("initial error");
        auto chained_err = err_result.and_then([](i32) {
            return Result<string>::ok("should not reach");
        });
        
        ASSERT_TRUE(chained_err.is_err());
        ASSERT_EQ(chained_err.error(), "initial error");
    TEST_END
    
    TEST_CASE("Result<void> specialization")
        auto ok_void = Result<void>::ok();
        auto err_void = Result<void>::err("void error");
        
        ASSERT_TRUE(ok_void.is_ok());
        ASSERT_FALSE(ok_void.is_err());
        
        ASSERT_FALSE(err_void.is_ok());
        ASSERT_TRUE(err_void.is_err());
        
        // Verify unwrap for void
        ok_void.unwrap(); // Should not throw an exception
        
        ASSERT_EQ(err_void.error(), "void error");
    TEST_END
    
    TEST_CASE("Custom error types")
        struct CustomError {
            i32 code;
            string message;
            
            bool operator==(const CustomError& other) const {
                return code == other.code && message == other.message;
            }
        };
        
        auto ok_result = Result<string, CustomError>::ok("success");
        auto err_result = Result<string, CustomError>::err({404, "Not found"});
        
        ASSERT_TRUE(ok_result.is_ok());
        ASSERT_TRUE(err_result.is_err());
        
        auto error = err_result.error();
        ASSERT_EQ(error.code, 404);
        ASSERT_EQ(error.message, "Not found");
    TEST_END
    
    RUN_ALL_TESTS();
} 