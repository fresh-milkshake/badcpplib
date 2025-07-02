#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_TIME_UTILS
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Time Utils Module Tests");
    
    TEST_CASE("Timer basic functionality")
        time_utils::Timer timer;
        
        ASSERT_FALSE(timer.is_running());
        ASSERT_EQ(timer.elapsed_ms(), 0);
        
        // Start and stop
        timer.start();
        ASSERT_TRUE(timer.is_running());
        
        // Small delay
        time_utils::sleep::milliseconds(10);
        
        auto elapsed = timer.stop();
        ASSERT_FALSE(timer.is_running());
        ASSERT_TRUE(timer.elapsed_ms() >= 5); // Account for timing variations
        ASSERT_TRUE(timer.elapsed_seconds() > 0.0);
        
        // Reset
        timer.reset();
        ASSERT_EQ(timer.elapsed_ms(), 0);
        ASSERT_FALSE(timer.is_running());
    TEST_END
    
    TEST_CASE("Timer accumulation")
        time_utils::Timer timer;
        
        // First interval
        timer.start();
        time_utils::sleep::milliseconds(10);
        timer.stop();
        auto first_elapsed = timer.elapsed_ms();
        
        // Second interval
        timer.start();
        time_utils::sleep::milliseconds(10);
        timer.stop();
        auto total_elapsed = timer.elapsed_ms();
        
        ASSERT_TRUE(total_elapsed >= first_elapsed);
        ASSERT_TRUE(total_elapsed >= 15); // Minimum 15ms total
    TEST_END
    
    TEST_CASE("ScopedTimer functionality")
        i64 elapsed_ms = 0;
        
        {
            time_utils::ScopedTimer scoped("test_operation", false);
            time_utils::sleep::milliseconds(10);
            elapsed_ms = scoped.elapsed_ms();
        } // Destructor called here
        
        ASSERT_TRUE(elapsed_ms >= 5);
    TEST_END
    
    TEST_CASE("Duration creation and conversion")
        // Create durations
        auto ms_duration = time_utils::duration::milliseconds(1000);
        auto us_duration = time_utils::duration::microseconds(1000000);
        auto ns_duration = time_utils::duration::nanoseconds(1000000000);
        auto sec_duration = time_utils::duration::seconds(1.0);
        
        // Conversions
        auto ms_value = time_utils::duration::to_milliseconds(sec_duration);
        auto us_value = time_utils::duration::to_microseconds(sec_duration);
        auto ns_value = time_utils::duration::to_nanoseconds(sec_duration);
        auto sec_value = time_utils::duration::to_seconds(ms_duration);
        
        ASSERT_EQ(ms_value, 1000);
        ASSERT_EQ(us_value, 1000000);
        ASSERT_EQ(ns_value, 1000000000);
        ASSERT_TRUE(std::abs(sec_value - 1.0) < 0.001); // Check with tolerance
        
        // Larger units
        auto min_duration = time_utils::duration::minutes(1.0);
        auto hour_duration = time_utils::duration::hours(1.0);
        
        ASSERT_EQ(time_utils::duration::to_seconds(min_duration), 60.0);
        ASSERT_EQ(time_utils::duration::to_seconds(hour_duration), 3600.0);
    TEST_END
    
    TEST_CASE("Current time functions")
        // Get current time
        auto now_point = time_utils::now::get();
        auto timestamp_ms = time_utils::now::timestamp_ms();
        auto timestamp_us = time_utils::now::timestamp_us();
        auto timestamp_ns = time_utils::now::timestamp_ns();
        auto system_time = time_utils::now::system_time();
        
        // Check that times are reasonable
        ASSERT_TRUE(timestamp_ms > 0);
        ASSERT_TRUE(timestamp_us > timestamp_ms * 1000);
        ASSERT_TRUE(timestamp_ns > timestamp_us * 1000);
        ASSERT_TRUE(system_time > 1600000000); // After year 2020
        
        // Check that times are close to each other
        time_utils::sleep::milliseconds(1);
        auto timestamp_ms2 = time_utils::now::timestamp_ms();
        ASSERT_TRUE(timestamp_ms2 > timestamp_ms);
    TEST_END
    
    TEST_CASE("Sleep functions")
        time_utils::Timer timer;
        
        // Test sleep milliseconds
        timer.start();
        time_utils::sleep::milliseconds(20);
        timer.stop();
        ASSERT_TRUE(timer.elapsed_ms() >= 15); // Minimum 15ms
        
        timer.reset();
        
        // Test sleep seconds
        timer.start();
        time_utils::sleep::seconds(0.02); // 20ms
        timer.stop();
        ASSERT_TRUE(timer.elapsed_ms() >= 15);
        
        timer.reset();
        
        // Test sleep for_duration
        auto duration = time_utils::duration::milliseconds(20);
        timer.start();
        time_utils::sleep::for_duration(duration);
        timer.stop();
        ASSERT_TRUE(timer.elapsed_ms() >= 15);
    TEST_END
    
    TEST_CASE("Time formatting")
        auto current_time = time_utils::now::system_time();
        
        // Time formatting
        auto time_str = time_utils::format::time_to_string(current_time);
        ASSERT_TRUE(time_str.length() > 10); // Should have reasonable length
        
        // Formatting with custom format
        auto custom_time = time_utils::format::time_to_string(current_time, "%Y-%m-%d");
        ASSERT_TRUE(custom_time.length() == 10); // YYYY-MM-DD
        
        // Current time
        auto current_str = time_utils::format::current_time();
        ASSERT_TRUE(current_str.length() > 10);
        
        // ISO8601 format
        auto iso_str = time_utils::format::iso8601(current_time);
        ASSERT_TRUE(iso_str.find('T') != string::npos); // Should contain T
        
        auto iso_now = time_utils::format::iso8601_now();
        ASSERT_TRUE(iso_now.find('T') != string::npos);
    TEST_END
    
    TEST_CASE("Duration formatting")
        // Format various durations
        auto ms_duration = time_utils::duration::milliseconds(1500);
        auto sec_duration = time_utils::duration::seconds(65.5);
        auto min_duration = time_utils::duration::minutes(125.3);
        
        auto ms_str = time_utils::format::duration_to_string(ms_duration);
        auto sec_str = time_utils::format::duration_to_string(sec_duration);
        auto min_str = time_utils::format::duration_to_string(min_duration);
        
        // Check that formats return reasonable strings
        ASSERT_TRUE(ms_str.length() > 0);
        ASSERT_TRUE(sec_str.length() > 0);
        ASSERT_TRUE(min_str.length() > 0);
    TEST_END
    
    TEST_CASE("Benchmark functions")
        // Simple function for benchmark
        auto test_func = []() {
            volatile i32 sum = 0;
            for (i32 i = 0; i < 1000; ++i) {
                sum += i;
            }
            return sum;
        };
        
        // Single benchmark
        auto duration = time_utils::benchmark(test_func);
        ASSERT_TRUE(time_utils::duration::to_nanoseconds(duration) > 0);
        
        // Benchmark with result
        auto [bench_duration, result] = time_utils::benchmark_with_result(test_func);
        ASSERT_TRUE(time_utils::duration::to_nanoseconds(bench_duration) > 0);
        ASSERT_EQ(result, 499500); // Sum of 0+1+2+...+999
        
        // Average benchmark
        auto avg_duration = time_utils::average_benchmark(test_func, 10);
        ASSERT_TRUE(time_utils::duration::to_nanoseconds(avg_duration) > 0);
    TEST_END
    
    TEST_CASE("PerformanceCounter")
        time_utils::PerformanceCounter counter;
        
        ASSERT_EQ(counter.get_frame_count(), 0u);
        ASSERT_EQ(counter.get_fps(), 0.0);
        
        // Simulate several frames
        for (usize i = 0; i < 5; ++i) {
            counter.tick();
            time_utils::sleep::milliseconds(20); // 50 FPS simulation
        }
        
        ASSERT_EQ(counter.get_frame_count(), 5u);
        auto fps = counter.get_fps();
        ASSERT_TRUE(fps >= 0.0); // FPS should be non-negative
        
        // Reset
        counter.reset();
        ASSERT_EQ(counter.get_frame_count(), 0u);
    TEST_END
    
    RUN_ALL_TESTS();
} 