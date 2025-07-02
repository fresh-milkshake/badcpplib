#include "badcpplib.hpp"
#include <cassert>
#include <iostream>

using namespace badcpplib;

void test_basic_types() {
    std::cout << "Testing basic types..." << std::endl;
    
    i32 number = 42;
    f64 pi = 3.14159;
    string text = "test";
    
    assert(number == 42);
    assert(pi > 3.14);
    assert(text == "test");
    
    std::cout << "✓ Basic types work correctly" << std::endl;
}

void test_result_type() {
    std::cout << "Testing Result type..." << std::endl;
    
    auto success = Result<i32>::ok(42);
    auto error = Result<i32>::err("Testing error");
    
    assert(success.is_ok());
    assert(!success.is_err());
    assert(success.unwrap() == 42);
    
    assert(!error.is_ok());
    assert(error.is_err());
    assert(error.error() == "Testing error");
    
    std::cout << "✓ Result type works correctly" << std::endl;
}

void test_string_utils() {
    std::cout << "Testing string utilities..." << std::endl;
    
    string text = "hello,world,test";
    auto parts = string_utils::split(text, ',');
    
    assert(parts.size() == 3);
    assert(parts[0] == "hello");
    assert(parts[1] == "world");
    assert(parts[2] == "test");
    
    auto joined = string_utils::join(parts, "|");
    assert(joined == "hello|world|test");
    
    string spaces = "  trim test  ";
    string trimmed = string_utils::trim(spaces);
    assert(trimmed == "trim test");
    
    std::cout << "✓ String utilities work correctly" << std::endl;
}

void test_math_utils() {
    std::cout << "Testing mathematical utilities..." << std::endl;
    
    assert(math_utils::clamp(150, 0, 100) == 100);
    assert(math_utils::clamp(-10, 0, 100) == 0);
    assert(math_utils::clamp(50, 0, 100) == 50);
    
    assert(math_utils::lerp(0.0, 100.0, 0.5) == 50.0);
    
    assert(math_utils::is_power_of_two(16));
    assert(!math_utils::is_power_of_two(15));
    
    assert(math_utils::next_power_of_two(15) == 16);
    
    std::cout << "✓ Mathematical utilities work correctly" << std::endl;
}

void test_containers() {
    std::cout << "Testing containers..." << std::endl;
    
    // Test StaticArray
    containers::StaticArray<i32, 3> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    
    assert(arr.size() == 3);
    assert(arr[0] == 10);
    assert(arr[1] == 20);
    assert(arr[2] == 30);
    
    // Test CircularBuffer
    containers::CircularBuffer<string> buffer(2);
    buffer.push("first");
    buffer.push("second");
    
    assert(buffer.size() == 2);
    assert(buffer.pop() == "first");
    assert(buffer.pop() == "second");
    assert(buffer.size() == 0);
    
    std::cout << "✓ Containers work correctly" << std::endl;
}

int main() {
    std::cout << "=== Running BadCppLib basic tests ===" << std::endl;
    
    try {
        test_basic_types();
        test_result_type();
        test_string_utils();
        test_math_utils();
        test_containers();
        
        std::cout << "\n✅ All tests passed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
} 