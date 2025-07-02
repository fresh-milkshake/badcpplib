#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_CONTAINERS
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Containers Module Tests");
    
    TEST_CASE("StaticArray basic functionality")
        containers::StaticArray<i32, 5> arr;
        
        ASSERT_EQ(arr.size(), 5u);
        ASSERT_FALSE(arr.empty());
        
        // Filling and access
        for (usize i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<i32>(i * 10);
        }
        
        ASSERT_EQ(arr[0], 0);
        ASSERT_EQ(arr[1], 10);
        ASSERT_EQ(arr[4], 40);
        
        // at() with bounds checking
        ASSERT_EQ(arr.at(2), 20);
        ASSERT_THROWS(arr.at(10), std::out_of_range);
        
        // front/back
        ASSERT_EQ(arr.front(), 0);
        ASSERT_EQ(arr.back(), 40);
    TEST_END
    
    TEST_CASE("StaticArray initialization")
        containers::StaticArray<string, 3> arr{"hello", "world", "test"};
        
        ASSERT_EQ(arr.size(), 3u);
        ASSERT_EQ(arr[0], "hello");
        ASSERT_EQ(arr[1], "world");
        ASSERT_EQ(arr[2], "test");
        
        // Too many initializers
        ASSERT_THROWS((containers::StaticArray<i32, 2>{1, 2, 3, 4}), std::out_of_range);
    TEST_END
    
    TEST_CASE("StaticArray iterators")
        containers::StaticArray<i32, 4> arr{10, 20, 30, 40};
        
        // Iterator
        i32 sum = 0;
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            sum += *it;
        }
        ASSERT_EQ(sum, 100);
        
        // Range-based for
        sum = 0;
        for (const auto& value : arr) {
            sum += value;
        }
        ASSERT_EQ(sum, 100);
        
        // fill
        arr.fill(42);
        for (const auto& value : arr) {
            ASSERT_EQ(value, 42);
        }
    TEST_END
    
    TEST_CASE("CircularBuffer basic functionality")
        containers::CircularBuffer<i32> buffer(3);
        
        ASSERT_EQ(buffer.capacity(), 3u);
        ASSERT_EQ(buffer.size(), 0u);
        ASSERT_TRUE(buffer.empty());
        ASSERT_FALSE(buffer.full());
        
        // Adding elements
        buffer.push(10);
        buffer.push(20);
        buffer.push(30);
        
        ASSERT_EQ(buffer.size(), 3u);
        ASSERT_TRUE(buffer.full());
        ASSERT_EQ(buffer.front(), 10);
        ASSERT_EQ(buffer.back(), 30);
        
        // Overflow (overwrite)
        buffer.push(40);
        ASSERT_EQ(buffer.size(), 3u);
        ASSERT_EQ(buffer.front(), 20); // First element shifted
        ASSERT_EQ(buffer.back(), 40);
    TEST_END
    
    TEST_CASE("CircularBuffer pop operations")
        containers::CircularBuffer<string> buffer(2);
        
        buffer.push("first");
        buffer.push("second");
        
        ASSERT_EQ(buffer.pop(), "first");
        ASSERT_EQ(buffer.size(), 1u);
        ASSERT_EQ(buffer.front(), "second");
        
        ASSERT_EQ(buffer.pop(), "second");
        ASSERT_TRUE(buffer.empty());
        
        // Pop from empty buffer
        ASSERT_THROWS(buffer.pop(), std::runtime_error);
        ASSERT_THROWS(buffer.front(), std::runtime_error);
    TEST_END
    
    TEST_CASE("Stack operations")
        containers::Stack<i32> stack;
        
        ASSERT_TRUE(stack.empty());
        ASSERT_EQ(stack.size(), 0u);
        
        // Push elements
        stack.push(10);
        stack.push(20);
        stack.push(30);
        
        ASSERT_EQ(stack.size(), 3u);
        ASSERT_FALSE(stack.empty());
        ASSERT_EQ(stack.top(), 30);
        
        // Pop elements (LIFO)
        ASSERT_EQ(stack.pop(), 30);
        ASSERT_EQ(stack.pop(), 20);
        ASSERT_EQ(stack.size(), 1u);
        ASSERT_EQ(stack.top(), 10);
        
        ASSERT_EQ(stack.pop(), 10);
        ASSERT_TRUE(stack.empty());
        
        // Operations with empty stack
        ASSERT_THROWS(stack.pop(), std::runtime_error);
        ASSERT_THROWS(stack.top(), std::runtime_error);
    TEST_END
    
    TEST_CASE("Queue operations")
        containers::Queue<string> queue;
        
        ASSERT_TRUE(queue.empty());
        ASSERT_EQ(queue.size(), 0u);
        
        // Push elements
        queue.push("first");
        queue.push("second");
        queue.push("third");
        
        ASSERT_EQ(queue.size(), 3u);
        ASSERT_FALSE(queue.empty());
        ASSERT_EQ(queue.front(), "first");
        ASSERT_EQ(queue.back(), "third");
        
        // Pop elements (FIFO)
        ASSERT_EQ(queue.pop(), "first");
        ASSERT_EQ(queue.front(), "second");
        ASSERT_EQ(queue.size(), 2u);
        
        ASSERT_EQ(queue.pop(), "second");
        ASSERT_EQ(queue.pop(), "third");
        ASSERT_TRUE(queue.empty());
        
        // Operations with empty queue
        ASSERT_THROWS(queue.pop(), std::runtime_error);
        ASSERT_THROWS(queue.front(), std::runtime_error);
    TEST_END
    
    TEST_CASE("SimpleHashMap basic operations")
        containers::SimpleHashMap<string, i32> map;
        
        ASSERT_TRUE(map.empty());
        ASSERT_EQ(map.size(), 0u);
        
        // Insertion
        map.insert("hello", 42);
        map.insert("world", 100);
        
        ASSERT_EQ(map.size(), 2u);
        ASSERT_FALSE(map.empty());
        
        // Search
        ASSERT_TRUE(map.contains("hello"));
        ASSERT_TRUE(map.contains("world"));
        ASSERT_FALSE(map.contains("missing"));
        
        auto* value = map.find("hello");
        ASSERT_TRUE(value != nullptr);
        ASSERT_EQ(*value, 42);
        
        ASSERT_TRUE(map.find("missing") == nullptr);
    TEST_END
    
    TEST_CASE("SimpleHashMap operator[] and updates")
        containers::SimpleHashMap<i32, string> map;
        
        // Operator[] for insertion
        map[1] = "one";
        map[2] = "two";
        map[3] = "three";
        
        ASSERT_EQ(map.size(), 3u);
        ASSERT_TRUE(map.contains(1));
        ASSERT_TRUE(map.contains(2));
        ASSERT_TRUE(map.contains(3));
        
        // Operator[] for access
        ASSERT_EQ(map[1], "one");
        ASSERT_EQ(map[2], "two");
        
        // Update value
        map.insert(1, "ONE");
        ASSERT_EQ(*map.find(1), "ONE");
        
        // Deletion
        ASSERT_TRUE(map.erase(2));
        ASSERT_FALSE(map.contains(2));
        ASSERT_FALSE(map.erase(999)); // Non-existent key
        
        ASSERT_EQ(map.size(), 2u);
    TEST_END
    
    TEST_CASE("Container move semantics")
        // Stack with move
        containers::Stack<unique_ptr<i32>> stack;
        stack.push(make_unique<i32>(42));
        stack.push(make_unique<i32>(100));
        
        auto ptr = stack.pop();
        ASSERT_EQ(*ptr, 100);
        ASSERT_EQ(stack.size(), 1u);
        
        // CircularBuffer with move
        containers::CircularBuffer<unique_ptr<string>> buffer(2);
        buffer.push(make_unique<string>("hello"));
        buffer.push(make_unique<string>("world"));
        
        auto str_ptr = buffer.pop();
        ASSERT_EQ(*str_ptr, "hello");
    TEST_END
    
    TEST_CASE("Container edge cases")
        // CircularBuffer with zero capacity
        ASSERT_THROWS(containers::CircularBuffer<i32>(0), std::invalid_argument);
        
        // StaticArray of size 0
        containers::StaticArray<i32, 0> empty_arr;
        ASSERT_EQ(empty_arr.size(), 0u);
        ASSERT_TRUE(empty_arr.empty());
        
        // HashMap with multiple operations (rehashing test)
        containers::SimpleHashMap<i32, i32> big_map;
        for (i32 i = 0; i < 50; ++i) {
            big_map.insert(i, i * i);
        }
        
        ASSERT_EQ(big_map.size(), 50u);
        for (i32 i = 0; i < 50; ++i) {
            ASSERT_TRUE(big_map.contains(i));
            ASSERT_EQ(*big_map.find(i), i * i);
        }
    TEST_END
    
    RUN_ALL_TESTS();
} 