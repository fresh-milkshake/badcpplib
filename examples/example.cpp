#include "badcpplib.hpp"

using namespace badcpplib;

int main() {
    debug::print("=== BADCPPLIB DEMONSTRATION ===");
    
    // ===== CORE TYPES DEMONSTRATION =====
    debug::print("", "=== CORE TYPES ===");
    
    i32 integer = 42;
    f64 floating = 3.14159;
    string text = "Hello, World!";
    
    debug::print("Integer:", integer);
    debug::print("Floating point:", floating);
    debug::print("String:", text);

    // ===== RESULT TYPE DEMONSTRATION =====
    debug::print("", "=== RESULT TYPE ===");
    
    // First try to read a non-existent file
    auto file_result = file_utils::read_file("nonexistent.txt");
    if (file_result.is_err()) {
        debug::print("Error:", file_result.error());
    }
    
    // Create and write to a test file
    auto write_result = file_utils::write_file("test.txt", "Test file content for demonstration");
    if (write_result.is_ok()) {
        debug::print("File successfully created");
    }
    
    // Read back the content
    auto read_result = file_utils::read_file("test.txt");
    if (read_result.is_ok()) {
        debug::print("Read from file:", read_result.unwrap());
    }

    // ===== STRING UTILITIES DEMONSTRATION =====
    debug::print("", "=== STRING UTILITIES ===");
    
    string original = "  Hello, World! This is a test.  ";
    debug::print("Original string:", "'" + original + "'");
    
    string trimmed = string_utils::trim(original);
    debug::print("After trim:", "'" + trimmed + "'");
    
    string upper = string_utils::to_upper(trimmed);
    debug::print("Upper case:", " ", upper);
    
    string lower = string_utils::to_lower(trimmed);
    debug::print("Lower case:", " ", lower);
    
    auto split_words = string_utils::split("apple,banana,cherry", ',');
    debug::print("String splitting:", string_utils::join(split_words, " "));
    
    string joined = string_utils::join(split_words, " | ");
    debug::print("Joining:", joined);
    
    debug::print("Contains 'World'?", string_utils::contains(original, "World") ? "Yes" : "No");
    debug::print("Starts with '  Hello'?", string_utils::starts_with(original, "  Hello") ? "Yes" : "No");
    debug::print("Ends with 'test.  '?", string_utils::ends_with(original, "test.  ") ? "Yes" : "No");

    // ===== MATHEMATICAL UTILITIES DEMONSTRATION =====
    debug::print("", "=== MATHEMATICAL UTILITIES ===");
    
    i32 rand_int = math_utils::random_int(1, 100);
    f64 rand_double = math_utils::random_double(0.0, 1.0);
    
    debug::print("Random integer 1-100:", rand_int);
    debug::print("Random double 0.0-1.0:", rand_double);
    
    debug::print("Clamp(150, 0, 100):", math_utils::clamp(150, 0, 100));
    debug::print("Lerp(0, 100, 0.5):", math_utils::lerp(0, 100, 0.5));
    debug::print("Is 16 power of two?", math_utils::is_power_of_two(16) ? "Yes" : "No");
    debug::print("Next power of two for 15:", math_utils::next_power_of_two(15));

    // ===== TIME UTILITIES DEMONSTRATION =====
    debug::print("", "=== TIME UTILITIES ===");
    
    // Measure execution time of a simple operation
    f64 measured_time = time_utils::measure_time([]() {
        volatile i32 sum = 0;
        for (i32 i = 0; i < 10000; ++i) {
            sum += i;
        }
    });
    debug::print("Execution time:", measured_time, "seconds");
    
    // Timer usage
    time_utils::Timer timer;
    time_utils::sleep_ms(100);
    debug::print("Elapsed seconds:", timer.elapsed_seconds());

    // ===== CONTAINERS DEMONSTRATION =====
    debug::print("", "=== CONTAINERS ===");
    
    // StaticArray
    debug::print("-- StaticArray --");
    containers::StaticArray<i32, 10> arr;
    arr.push(10);
    arr.push(20);
    arr.push(30);
    
    debug::print("StaticArray size:", arr.size());
    debug::print("StaticArray elements:", arr[0], arr[1], arr[2]);
    
    // CircularBuffer
    debug::print("-- CircularBuffer --");
    containers::CircularBuffer<string> buffer(3);
    buffer.push("first");
    buffer.push("second");
    buffer.push("third");
    buffer.push("fourth"); // This overwrites "first"
    
    debug::print("CircularBuffer size:", buffer.size());
    debug::print("Extracting:", buffer.pop().value(), buffer.pop().value(), buffer.pop().value());

    // ===== FUNCTIONAL PROGRAMMING DEMONSTRATION =====
    debug::print("", "=== FUNCTIONAL PROGRAMMING ===");
    
    vector<i32> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Filter even numbers
    auto evens = functional::filter(numbers, [](i32 n) { return n % 2 == 0; });
    debug::print("Even numbers:", string_utils::join(
        functional::map(evens, [](i32 n) { return std::to_string(n); }), " "));
    
    // Map to squares
    auto squares = functional::map(numbers, [](i32 n) { return n * n; });
    debug::print("Squared numbers:", string_utils::join(
        functional::map(squares, [](i32 n) { return std::to_string(n); }), " "));
    
    // Reduce to sum
    i32 sum = functional::reduce(numbers, 0, [](i32 acc, i32 n) { return acc + n; });
    debug::print("Sum of all numbers:", sum);

    // ===== MEMORY MANAGEMENT DEMONSTRATION =====
    debug::print("", "=== MEMORY MANAGEMENT ===");
    
    // Smart pointers
    debug::print("-- Smart Pointers --");
    auto unique_ptr = memory::make_unique<string>("Unique pointer test");
    debug::print("Unique pointer:", *unique_ptr);
    
    auto shared_ptr = memory::make_shared<i32>(42);
    debug::print("Shared pointer:", *shared_ptr);
    
    // ScopeGuard
    debug::print("-- ScopeGuard --");
    {
        debug::print("Entering scope");
        SCOPE_EXIT(debug::print("Exiting scope"));
        debug::print("Inside scope");
    }

    // ===== DEBUG UTILITIES DEMONSTRATION =====
    debug::print("", "=== DEBUG UTILITIES ===");
    
    vector<string> fruits = {"apple", "banana", "cherry"};
    debug::print("Fruits container:", string_utils::join(fruits, " "));
    
    debug::assert_true(2 + 2 == 4, "Basic math should work");
    debug::print("Assertion passed");
    
    try {
        debug::assert_true(false, "This should throw an exception");
    } catch (const std::exception& e) {
        debug::print("Caught exception:", e.what());
    }

    // ===== FILE UTILITIES DEMONSTRATION =====
    debug::print("", "=== FILE UTILITIES ===");
    
    debug::print("test.txt exists?", file_utils::file_exists("test.txt") ? "Yes" : "No");

    // ===== CLEANUP =====
    debug::print("", "=== CLEANUP ===");
    debug::print("Test file remains: test.txt (manual cleanup may be needed)");

    // ===== FINAL MESSAGE =====
    debug::print("", "=== DEMONSTRATION COMPLETED ===");
    debug::print("BadCppLib provides essential utilities for modern C++ development!");

    return 0;
} 