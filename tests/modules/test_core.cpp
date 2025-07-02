#define BADCPPLIB_ENABLE_CORE
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Core Module Tests");
    
    TEST_CASE("Basic integer types")
        i8 val_i8 = -128;
        i16 val_i16 = -32768;
        i32 val_i32 = -2147483648;
        i64 val_i64 = -9223372036854775807LL;
        
        u8 val_u8 = 255;
        u16 val_u16 = 65535;
        u32 val_u32 = 4294967295U;
        u64 val_u64 = 18446744073709551615ULL;
        
        ASSERT_EQ(sizeof(i8), 1u);
        ASSERT_EQ(sizeof(i16), 2u);
        ASSERT_EQ(sizeof(i32), 4u);
        ASSERT_EQ(sizeof(i64), 8u);
        
        ASSERT_EQ(sizeof(u8), 1u);
        ASSERT_EQ(sizeof(u16), 2u);
        ASSERT_EQ(sizeof(u32), 4u);
        ASSERT_EQ(sizeof(u64), 8u);
        
        ASSERT_EQ(val_i8, -128);
        ASSERT_EQ(val_u8, 255);
    TEST_END
    
    TEST_CASE("Floating point types")
        f32 val_f32 = 3.14f;
        f64 val_f64 = 3.141592653589793;
        f128 val_f128 = 3.141592653589793238L;
        
        ASSERT_EQ(sizeof(f32), 4u);
        ASSERT_EQ(sizeof(f64), 8u);
        
        ASSERT_TRUE(val_f32 > 3.0f);
        ASSERT_TRUE(val_f64 > 3.0);
        ASSERT_TRUE(val_f128 > 3.0L);
    TEST_END
    
    TEST_CASE("Size types")
        usize size_val = 42;
        isize signed_size = -42;
        
        ASSERT_EQ(size_val, 42u);
        ASSERT_EQ(signed_size, -42);
        
        // Check that types have correct size
        static_assert(sizeof(usize) == sizeof(std::size_t));
        static_assert(sizeof(isize) == sizeof(std::ptrdiff_t));
    TEST_END
    
    TEST_CASE("String type")
        string str = "Hello, World!";
        
        ASSERT_EQ(str, "Hello, World!");
        ASSERT_EQ(str.length(), 13u);
        ASSERT_FALSE(str.empty());
        
        string empty_str;
        ASSERT_TRUE(empty_str.empty());
    TEST_END
    
    TEST_CASE("Smart pointers")
        // unique_ptr
        auto unique = make_unique<i32>(42);
        ASSERT_TRUE(unique != nullptr);
        ASSERT_EQ(*unique, 42);
        
        // shared_ptr
        auto shared1 = make_shared<string>("test");
        auto shared2 = shared1;
        ASSERT_EQ(shared1.use_count(), 2);
        ASSERT_EQ(*shared1, "test");
        ASSERT_EQ(*shared2, "test");
        
        // weak_ptr
        weak_ptr<string> weak = shared1;
        ASSERT_FALSE(weak.expired());
        auto locked = weak.lock();
        ASSERT_TRUE(locked != nullptr);
        ASSERT_EQ(*locked, "test");
    TEST_END
    
    TEST_CASE("Type compatibility")
        // Check compatibility with standard types
        static_assert(std::is_same_v<i32, std::int32_t>);
        static_assert(std::is_same_v<u64, std::uint64_t>);
        static_assert(std::is_same_v<f32, float>);
        static_assert(std::is_same_v<f64, double>);
        static_assert(std::is_same_v<string, std::string>);
        
        ASSERT_TRUE(true); // Compilation test
    TEST_END
    
    RUN_ALL_TESTS();
} 