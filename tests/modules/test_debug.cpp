#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_DEBUG
#define BADCPPLIB_ENABLE_TIME_UTILS
#define BADCPPLIB_ENABLE_FILE_UTILS
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Debug Module Tests");
    
    TEST_CASE("LogLevel utilities")
        // Test function for converting level to string
        ASSERT_EQ(debug::log_level_to_string(debug::LogLevel::TRACE), "TRACE");
        ASSERT_EQ(debug::log_level_to_string(debug::LogLevel::DEBUG), "DEBUG");
        ASSERT_EQ(debug::log_level_to_string(debug::LogLevel::INFO), "INFO");
        ASSERT_EQ(debug::log_level_to_string(debug::LogLevel::WARNING), "WARNING");
        ASSERT_EQ(debug::log_level_to_string(debug::LogLevel::ERROR), "ERROR");
        ASSERT_EQ(debug::log_level_to_string(debug::LogLevel::CRITICAL), "CRITICAL");
        
        // Test function for getting color
        auto trace_color = debug::get_log_color(debug::LogLevel::TRACE);
        auto error_color = debug::get_log_color(debug::LogLevel::ERROR);
        ASSERT_TRUE(!trace_color.empty());
        ASSERT_TRUE(!error_color.empty());
        ASSERT_NE(trace_color, error_color);
    TEST_END
    
    TEST_CASE("Logger basic functionality")
        debug::Logger logger(debug::LogLevel::DEBUG, false); // Without colors for tests
        
        // Check settings
        ASSERT_EQ(logger.get_min_level(), debug::LogLevel::DEBUG);
        ASSERT_FALSE(logger.colors_enabled());
        
        // Change settings
        logger.set_min_level(debug::LogLevel::WARNING);
        ASSERT_EQ(logger.get_min_level(), debug::LogLevel::WARNING);
        
        logger.enable_colors(true);
        ASSERT_TRUE(logger.colors_enabled());
        
        // Basic logging (visual output check)
        logger.trace("This trace should not appear");
        logger.debug("This debug should not appear");
        logger.warning("This warning should appear");
        logger.error("This error should appear");
        logger.critical("This critical should appear");
    TEST_END
    
    TEST_CASE("Logger convenience methods")
        debug::Logger logger(debug::LogLevel::TRACE, false);
        
        // Test all logging methods
        logger.trace("Trace message");
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warning("Warning message");
        logger.error("Error message");
        logger.critical("Critical message");
        
        // Logging with file and line information
        logger.info("Message with location", "test_file.cpp", 42);
    TEST_END
    
    TEST_CASE("Logger file output")
        const string log_filename = "test_log.txt";
        debug::Logger logger(debug::LogLevel::INFO, false);
        
        // Enable file logging
        bool file_enabled = logger.enable_file_logging(log_filename);
        ASSERT_TRUE(file_enabled);
        
        // Write several messages
        logger.info("First log message");
        logger.warning("Warning message");
        logger.error("Error message");
        
        // Disable file logging
        logger.disable_file_logging();
        
        // Check that file exists and contains data
        ASSERT_TRUE(file_utils::file_exists(log_filename));
        
        auto content_result = file_utils::read_file(log_filename);
        ASSERT_TRUE(content_result.is_ok());
        
        string content = content_result.unwrap();
        ASSERT_TRUE(content.find("First log message") != string::npos);
        ASSERT_TRUE(content.find("Warning message") != string::npos);
        ASSERT_TRUE(content.find("Error message") != string::npos);
        
        // Cleanup
        file_utils::remove_file(log_filename);
    TEST_END
    
    TEST_CASE("Global logger")
        auto& global_logger = debug::get_global_logger();
        
        // Change global logger settings
        auto old_level = global_logger.get_min_level();
        global_logger.set_min_level(debug::LogLevel::ERROR);
        
        // Macro testing
        LOG_TRACE("This trace should not appear");
        LOG_DEBUG("This debug should not appear");
        LOG_INFO("This info should not appear");
        LOG_WARNING("This warning should not appear");
        LOG_ERROR("This error should appear");
        LOG_CRITICAL("This critical should appear");
        
        // Restore settings
        global_logger.set_min_level(old_level);
    TEST_END
    
    TEST_CASE("Profiler functionality")
        debug::Profiler profiler;
        
        // Profile simple operation
        profiler.begin_profile("test_operation");
        
        // Simulate work
        volatile i32 sum = 0;
        for (i32 i = 0; i < 10000; ++i) {
            sum += i;
        }
        
        auto duration = time_utils::duration::microseconds(100); // Fake time
        profiler.end_profile("test_operation", duration);
        
        // Re-profile the same operation
        profiler.begin_profile("test_operation");
        for (i32 i = 0; i < 5000; ++i) {
            sum += i;
        }
        auto duration2 = time_utils::duration::microseconds(50);
        profiler.end_profile("test_operation", duration2);
        
        // Profile another operation
        profiler.begin_profile("another_operation");
        time_utils::sleep::milliseconds(1);
        auto duration3 = time_utils::duration::milliseconds(1);
        profiler.end_profile("another_operation", duration3);
        
        // Print report (visual check)
        profiler.print_report();
        
        // Clear data
        profiler.clear();
    TEST_END
    
    TEST_CASE("ScopedProfiler functionality")
        debug::Profiler& global_profiler = debug::get_global_profiler();
        global_profiler.clear();
        
        {
            debug::ScopedProfiler scoped("scoped_test");
            
            // Simulate work
            volatile f64 result = 0.0;
            for (i32 i = 0; i < 1000; ++i) {
                result += std::sin(i * 0.1);
            }
        } // Destructor called here
        
        // Check that profiling was recorded
        global_profiler.print_report();
    TEST_END
    
    TEST_CASE("AllocationTracker basic functionality")
        debug::memory::AllocationTracker tracker;
        
        // Check initial state
        ASSERT_EQ(tracker.get_allocation_count(), 0u);
        ASSERT_EQ(tracker.get_deallocation_count(), 0u);
        ASSERT_EQ(tracker.get_total_allocated_bytes(), 0u);
        ASSERT_EQ(tracker.get_current_allocated_bytes(), 0u);
        
        // Simulate memory allocation
        void* ptr1 = std::malloc(100);
        tracker.track_allocation(ptr1, 100);
        
        ASSERT_EQ(tracker.get_allocation_count(), 1u);
        ASSERT_EQ(tracker.get_total_allocated_bytes(), 100u);
        ASSERT_EQ(tracker.get_current_allocated_bytes(), 100u);
        
        void* ptr2 = std::malloc(200);
        tracker.track_allocation(ptr2, 200);
        
        ASSERT_EQ(tracker.get_allocation_count(), 2u);
        ASSERT_EQ(tracker.get_total_allocated_bytes(), 300u);
        ASSERT_EQ(tracker.get_current_allocated_bytes(), 300u);
        
        // Simulate memory deallocation
        tracker.track_deallocation(ptr1);
        std::free(ptr1);
        
        ASSERT_EQ(tracker.get_deallocation_count(), 1u);
        ASSERT_EQ(tracker.get_current_allocated_bytes(), 200u);
        
        tracker.track_deallocation(ptr2);
        std::free(ptr2);
        
        ASSERT_EQ(tracker.get_deallocation_count(), 2u);
        ASSERT_EQ(tracker.get_current_allocated_bytes(), 0u);
        
        // Print statistics
        tracker.print_statistics();
        
        // Reset counters
        tracker.reset();
        ASSERT_EQ(tracker.get_allocation_count(), 0u);
        ASSERT_EQ(tracker.get_total_allocated_bytes(), 0u);
    TEST_END
    
    TEST_CASE("Debug print functions")
        // Test debug_print for basic types
        ASSERT_EQ(debug::debug_print(42), "42");
        ASSERT_EQ(debug::debug_print(3.14), "3.14");
        ASSERT_EQ(debug::debug_print("hello"), "hello");
        
        // Test debug_print_container
        std::vector<i32> numbers = {1, 2, 3, 4, 5};
        auto container_str = debug::debug_print_container(numbers, "test_vector");
        
        ASSERT_TRUE(container_str.find("test_vector") != string::npos);
        ASSERT_TRUE(container_str.find("1") != string::npos);
        ASSERT_TRUE(container_str.find("5") != string::npos);
        ASSERT_TRUE(container_str.find("5") != string::npos); // Size
        
        // Empty container
        std::vector<i32> empty;
        auto empty_str = debug::debug_print_container(empty, "empty_vector");
        ASSERT_TRUE(empty_str.find("empty") != string::npos || empty_str.find("0") != string::npos);
    TEST_END
    
    TEST_CASE("Hex dump functionality")
        // Test data
        u8 test_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                          0x72, 0x6C, 0x64, 0x21, 0x00, 0xFF, 0xAA, 0x55};
        
        // Hex dump with various parameters
        auto dump1 = debug::hex_dump(test_data, sizeof(test_data), 8);
        auto dump2 = debug::hex_dump(test_data, sizeof(test_data), 16);
        
        ASSERT_TRUE(!dump1.empty());
        ASSERT_TRUE(!dump2.empty());
        
        // Check that dump contains hexadecimal numbers
        ASSERT_TRUE(dump1.find("48") != string::npos); // First byte
        ASSERT_TRUE(dump1.find("FF") != string::npos || dump1.find("ff") != string::npos);
        
        // Small data
        u8 small_data[] = {0x01, 0x02, 0x03};
        auto small_dump = debug::hex_dump(small_data, sizeof(small_data));
        ASSERT_TRUE(!small_dump.empty());
        ASSERT_TRUE(small_dump.find("01") != string::npos || small_dump.find("01") != string::npos);
    TEST_END
    
    TEST_CASE("Debug assertions (non-fatal tests)")
        // These tests check that macros compile correctly
        // In release build ASSERT will be disabled
        
        #ifndef NDEBUG
        // In debug build we can test valid assertions
        ASSERT(true);
        ASSERT_MSG(1 + 1 == 2, "Basic math should work");
        #endif
        
        // Test that macros compile
        bool test_condition = true;
        ASSERT(test_condition);
        ASSERT_MSG(test_condition, "Test condition should be true");
    TEST_END
    
    TEST_CASE("Thread safety test")
        debug::Logger logger(debug::LogLevel::INFO, false);
        debug::memory::AllocationTracker tracker;
        
        // Simple thread safety test
        const usize num_threads = 4;
        const usize operations_per_thread = 100;
        
        std::vector<std::thread> threads;
        
        for (usize i = 0; i < num_threads; ++i) {
            threads.emplace_back([&logger, &tracker, i, operations_per_thread]() {
                for (usize j = 0; j < operations_per_thread; ++j) {
                    // Logging
                    logger.info("Thread " + std::to_string(i) + " operation " + std::to_string(j));
                    
                    // Track allocations
                    void* ptr = std::malloc(64);
                    tracker.track_allocation(ptr, 64);
                    tracker.track_deallocation(ptr);
                    std::free(ptr);
                }
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Check results
        usize total_allocations = num_threads * operations_per_thread;
        ASSERT_EQ(tracker.get_allocation_count(), total_allocations);
        ASSERT_EQ(tracker.get_deallocation_count(), total_allocations);
        ASSERT_EQ(tracker.get_current_allocated_bytes(), 0u);
    TEST_END
    
    RUN_ALL_TESTS();
} 