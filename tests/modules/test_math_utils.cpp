#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_MATH
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"
#include <cmath>

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Math Utils Module Tests");
    
    TEST_CASE("Random class basic functionality")
        math_utils::Random rng(12345);
        
        // Range test for integers
        for (int i = 0; i < 100; ++i) {
            i32 val = rng.next_int(1, 10);
            ASSERT_TRUE(val >= 1 && val <= 10);
        }
        
        // Range test for float
        for (int i = 0; i < 100; ++i) {
            f32 val = rng.next_float(0.0f, 1.0f);
            ASSERT_TRUE(val >= 0.0f && val <= 1.0f);
        }
        
        // Range test for double
        for (int i = 0; i < 100; ++i) {
            f64 val = rng.next_double(5.0, 15.0);
            ASSERT_TRUE(val >= 5.0 && val <= 15.0);
        }
    TEST_END
    
    TEST_CASE("Random reproducibility with seed")
        math_utils::Random rng1(42);
        math_utils::Random rng2(42);
        
        // The same seed should produce identical sequences
        for (int i = 0; i < 10; ++i) {
            ASSERT_EQ(rng1.next_int(0, 1000), rng2.next_int(0, 1000));
        }
        
        // Different seeds should produce different values
        math_utils::Random rng3(999);
        bool different = false;
        for (int i = 0; i < 10; ++i) {
            if (rng1.next_int(0, 1000) != rng3.next_int(0, 1000)) {
                different = true;
                break;
            }
        }
        ASSERT_TRUE(different);
    TEST_END
    
    TEST_CASE("Power of two functions")
        // is_power_of_two
        ASSERT_TRUE(math_utils::is_power_of_two(1));   // 2^0
        ASSERT_TRUE(math_utils::is_power_of_two(2));   // 2^1
        ASSERT_TRUE(math_utils::is_power_of_two(4));   // 2^2
        ASSERT_TRUE(math_utils::is_power_of_two(8));   // 2^3
        ASSERT_TRUE(math_utils::is_power_of_two(16));  // 2^4
        ASSERT_TRUE(math_utils::is_power_of_two(1024)); // 2^10
        
        ASSERT_FALSE(math_utils::is_power_of_two(0));
        ASSERT_FALSE(math_utils::is_power_of_two(3));
        ASSERT_FALSE(math_utils::is_power_of_two(5));
        ASSERT_FALSE(math_utils::is_power_of_two(6));
        ASSERT_FALSE(math_utils::is_power_of_two(7));
        ASSERT_FALSE(math_utils::is_power_of_two(15));
        
        // next_power_of_two
        ASSERT_EQ(math_utils::next_power_of_two(1), 1);
        ASSERT_EQ(math_utils::next_power_of_two(2), 2);
        ASSERT_EQ(math_utils::next_power_of_two(3), 4);
        ASSERT_EQ(math_utils::next_power_of_two(5), 8);
        ASSERT_EQ(math_utils::next_power_of_two(9), 16);
        ASSERT_EQ(math_utils::next_power_of_two(100), 128);
        ASSERT_EQ(math_utils::next_power_of_two(1000), 1024);
        
        // prev_power_of_two
        ASSERT_EQ(math_utils::prev_power_of_two(1), 1);
        ASSERT_EQ(math_utils::prev_power_of_two(2), 2);
        ASSERT_EQ(math_utils::prev_power_of_two(3), 2);
        ASSERT_EQ(math_utils::prev_power_of_two(7), 4);
        ASSERT_EQ(math_utils::prev_power_of_two(15), 8);
        ASSERT_EQ(math_utils::prev_power_of_two(100), 64);
        ASSERT_EQ(math_utils::prev_power_of_two(1000), 512);
    TEST_END
    
    TEST_CASE("Mathematical constants")
        // Validate constant accuracy (with tolerance)
        const f64 epsilon = 1e-10;
        
        ASSERT_TRUE(std::abs(math_utils::PI - 3.14159265358979323846) < epsilon);
        ASSERT_TRUE(std::abs(math_utils::E - 2.71828182845904523536) < epsilon);
        ASSERT_TRUE(std::abs(math_utils::SQRT2 - 1.41421356237309504880) < epsilon);
        ASSERT_TRUE(std::abs(math_utils::SQRT3 - 1.73205080756887729352) < epsilon);
        ASSERT_TRUE(std::abs(math_utils::PHI - 1.61803398874989484820) < epsilon); // Golden ratio
        
        // Validate derived constants
        ASSERT_TRUE(std::abs(math_utils::PI_2 - math_utils::PI / 2.0) < epsilon);
        ASSERT_TRUE(std::abs(math_utils::PI_4 - math_utils::PI / 4.0) < epsilon);
        ASSERT_TRUE(std::abs(math_utils::TAU - math_utils::PI * 2.0) < epsilon);
    TEST_END
    
    TEST_CASE("Utility functions")
        // clamp
        ASSERT_EQ(math_utils::clamp(5, 1, 10), 5);     // within range
        ASSERT_EQ(math_utils::clamp(-5, 1, 10), 1);    // below min
        ASSERT_EQ(math_utils::clamp(15, 1, 10), 10);   // above max
        ASSERT_EQ(math_utils::clamp(1, 1, 10), 1);     // equal to min
        ASSERT_EQ(math_utils::clamp(10, 1, 10), 10);   // equal to max
        
        // lerp (linear interpolation)
        const f32 eps = 1e-6f;
        ASSERT_TRUE(std::abs(math_utils::lerp(0.0f, 10.0f, 0.0f) - 0.0f) < eps);
        ASSERT_TRUE(std::abs(math_utils::lerp(0.0f, 10.0f, 1.0f) - 10.0f) < eps);
        ASSERT_TRUE(std::abs(math_utils::lerp(0.0f, 10.0f, 0.5f) - 5.0f) < eps);
        ASSERT_TRUE(std::abs(math_utils::lerp(5.0f, 15.0f, 0.3f) - 8.0f) < eps);
        
        // abs for different types
        ASSERT_EQ(math_utils::abs(-5), 5);
        ASSERT_EQ(math_utils::abs(5), 5);
        ASSERT_EQ(math_utils::abs(0), 0);
        ASSERT_TRUE(std::abs(math_utils::abs(-5.5f) - 5.5f) < eps);
        
        // min/max
        ASSERT_EQ(math_utils::min(3, 7), 3);
        ASSERT_EQ(math_utils::min(7, 3), 3);
        ASSERT_EQ(math_utils::min(5, 5), 5);
        
        ASSERT_EQ(math_utils::max(3, 7), 7);
        ASSERT_EQ(math_utils::max(7, 3), 7);
        ASSERT_EQ(math_utils::max(5, 5), 5);
    TEST_END
    
    TEST_CASE("Sleep function")
        // Test sleep_ms function (check that it doesn't fail)
        auto start = std::chrono::high_resolution_clock::now();
        math_utils::sleep_ms(10); // 10 milliseconds
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        // Verify at least 10ms passed (allowing slight timer inaccuracy)
        ASSERT_TRUE(duration.count() >= 8); // With 2ms tolerance
    TEST_END
    
    TEST_CASE("Edge cases and bounds")
        math_utils::Random rng;
        
        // Test with identical bounds
        ASSERT_EQ(rng.next_int(5, 5), 5);
        
        // Test with negative numbers
        for (int i = 0; i < 50; ++i) {
            i32 val = rng.next_int(-10, -5);
            ASSERT_TRUE(val >= -10 && val <= -5);
        }
        
        // Test large numbers for power_of_two
        ASSERT_TRUE(math_utils::is_power_of_two(1ULL << 20));
        ASSERT_FALSE(math_utils::is_power_of_two((1ULL << 20) + 1));
        
        // Test clamp with negative numbers
        ASSERT_EQ(math_utils::clamp(-15, -10, -5), -10);
        ASSERT_EQ(math_utils::clamp(-3, -10, -5), -5);
        ASSERT_EQ(math_utils::clamp(-7, -10, -5), -7);
    TEST_END
    
    RUN_ALL_TESTS();
} 