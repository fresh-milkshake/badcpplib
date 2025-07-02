#pragma once

/**
 * @file containers.hpp
 * @brief Collection of efficient container classes with specialized behaviors
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides specialized container types optimized for specific use cases,
 * including static arrays, circular buffers, stacks, queues, and simple hash maps.
 * Enable with `BADCPPLIB_ENABLE_CONTAINERS`.
 */

#include "core.hpp"
#include <array>
#include <vector>
#include <memory>
#include <stdexcept>

namespace badcpplib {

/**
 * @namespace badcpplib::containers
 * @brief Specialized container classes for various data structure needs
 * 
 * This namespace provides a collection of container classes designed for 
 * specific use cases where standard library containers might not be optimal:
 * 
 * - **StaticArray**: Fixed-size array with compile-time size determination
 * - **CircularBuffer**: Ring buffer for streaming data and limited memory scenarios
 * - **Stack**: Simple LIFO container optimized for push/pop operations
 * - **Queue**: FIFO container with efficient front/back operations
 * - **SimpleHashMap**: Basic hash table for key-value storage
 * 
 * @section performance Performance Characteristics
 * 
 * All containers are designed with performance in mind:
 * - StaticArray: Zero-overhead abstraction over std::array
 * - CircularBuffer: O(1) insertions with fixed memory usage
 * - Stack/Queue: O(1) amortized operations
 * - SimpleHashMap: O(1) average case lookups with linear probing
 * 
 * @section thread_safety Thread Safety
 * None of these containers are thread-safe. External synchronization is required
 * for concurrent access.
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_CONTAINERS
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * // Static array with compile-time size
 * StaticArray<i32, 5> arr = {1, 2, 3, 4, 5};
 * 
 * // Circular buffer for streaming data
 * CircularBuffer<i32> buffer(100);
 * for (i32 i = 0; i < 150; ++i) {
 *     buffer.push(i);  // Automatically overwrites old data
 * }
 * 
 * // Simple stack operations
 * Stack<string> stack;
 * stack.push("hello");
 * stack.push("world");
 * string top = stack.pop();  // "world"
 * ```
 */
namespace containers {

/**
 * @brief Static array with compile-time fixed size
 * @tparam T The element type
 * @tparam N The array size (must be > 0)
 * 
 * A zero-overhead wrapper around std::array that provides additional
 * safety checks and a more consistent interface. The size is determined
 * at compile-time and cannot be changed.
 * 
 * @section memory Memory Layout
 * Elements are stored contiguously in memory, making this container
 * cache-friendly and suitable for high-performance applications.
 * 
 * @section bounds_checking Bounds Checking
 * The at() method performs bounds checking and throws on invalid access.
 * The operator[] method provides unchecked access for performance.
 * 
 * @usecase{Small fixed-size collections}
 * Perfect for coordinates (x, y, z), small lookup tables, or any
 * collection where the size is known at compile time.
 * 
 * @usecase{Performance-critical code}
 * Zero overhead compared to raw arrays, with better type safety.
 * 
 * @example
 * ```cpp
 * // 3D vector using StaticArray
 * StaticArray<f64, 3> position = {1.0, 2.0, 3.0};
 * position[0] = 10.0;  // Direct access
 * f64 y = position.at(1);  // Bounds-checked access
 * 
 * // Range-based loop support
 * for (f64& component : position) {
 *     component *= 2.0;
 * }
 * ```
 * 
 * @complexity{All operations are O(1)}
 * @threadsafe{No - external synchronization required}
 * @since{1.0.0}
 */
template<typename T, usize N>
class StaticArray {
private:
    std::array<T, N> data_; ///< Internal storage
    
public:
    // Type aliases for STL compatibility
    using value_type = T;                                          ///< Element type
    using size_type = usize;                                       ///< Size type
    using reference = T&;                                          ///< Reference type
    using const_reference = const T&;                              ///< Const reference type
    using iterator = typename std::array<T, N>::iterator;          ///< Iterator type
    using const_iterator = typename std::array<T, N>::const_iterator; ///< Const iterator type
    
    /**
     * @brief Default constructor - default-initializes all elements
     */
    StaticArray() = default;
    
    // List initialization
    StaticArray(std::initializer_list<T> init) {
        if (init.size() > N) {
            throw std::out_of_range("Too many initializers for StaticArray");
        }
        usize i = 0;
        for (const auto& item : init) {
            data_[i++] = item;
        }
    }
    
    // Element access
    reference operator[](size_type index) { return data_[index]; }
    const_reference operator[](size_type index) const { return data_[index]; }
    
    reference at(size_type index) {
        if (index >= N) throw std::out_of_range("StaticArray index out of range");
        return data_[index];
    }
    
    const_reference at(size_type index) const {
        if (index >= N) throw std::out_of_range("StaticArray index out of range");
        return data_[index];
    }
    
    // Size and capacity
    constexpr size_type size() const noexcept { return N; }
    constexpr size_type max_size() const noexcept { return N; }
    constexpr bool empty() const noexcept { return N == 0; }
    
    // Iterators
    iterator begin() noexcept { return data_.begin(); }
    const_iterator begin() const noexcept { return data_.begin(); }
    const_iterator cbegin() const noexcept { return data_.cbegin(); }
    
    iterator end() noexcept { return data_.end(); }
    const_iterator end() const noexcept { return data_.end(); }
    const_iterator cend() const noexcept { return data_.cend(); }
    
    // Access to first and last element
    reference front() { return data_.front(); }
    const_reference front() const { return data_.front(); }
    reference back() { return data_.back(); }
    const_reference back() const { return data_.back(); }
    
    // Filling
    void fill(const T& value) { data_.fill(value); }
    
    // Raw data
    T* data() noexcept { return data_.data(); }
    const T* data() const noexcept { return data_.data(); }
};

// Circular buffer (CircularBuffer)
template<typename T>
class CircularBuffer {
private:
    std::vector<T> buffer_;
    usize head_;
    usize tail_;
    usize size_;
    usize capacity_;
    
public:
    using value_type = T;
    using size_type = usize;
    using reference = T&;
    using const_reference = const T&;
    
    explicit CircularBuffer(size_type capacity) 
        : buffer_(capacity), head_(0), tail_(0), size_(0), capacity_(capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("CircularBuffer capacity must be > 0");
        }
    }
    
    // Adding element
    void push(const T& value) {
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % capacity_;
        
        if (size_ < capacity_) {
            ++size_;
        } else {
            // Buffer is full, overwrite old data
            head_ = (head_ + 1) % capacity_;
        }
    }
    
    void push(T&& value) {
        buffer_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % capacity_;
        
        if (size_ < capacity_) {
            ++size_;
        } else {
            head_ = (head_ + 1) % capacity_;
        }
    }
    
    // Element extraction
    T pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty CircularBuffer");
        }
        
        T result = std::move(buffer_[head_]);
        head_ = (head_ + 1) % capacity_;
        --size_;
        return result;
    }
    
    // Element access
    reference front() {
        if (empty()) throw std::runtime_error("CircularBuffer is empty");
        return buffer_[head_];
    }
    
    const_reference front() const {
        if (empty()) throw std::runtime_error("CircularBuffer is empty");
        return buffer_[head_];
    }
    
    reference back() {
        if (empty()) throw std::runtime_error("CircularBuffer is empty");
        usize back_index = (tail_ + capacity_ - 1) % capacity_;
        return buffer_[back_index];
    }
    
    const_reference back() const {
        if (empty()) throw std::runtime_error("CircularBuffer is empty");
        usize back_index = (tail_ + capacity_ - 1) % capacity_;
        return buffer_[back_index];
    }
    
    // Size and state
    size_type size() const noexcept { return size_; }
    size_type capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    bool full() const noexcept { return size_ == capacity_; }
    
    // Clearing
    void clear() noexcept {
        head_ = tail_ = size_ = 0;
    }
};

// Simple stack
template<typename T>
class Stack {
private:
    std::vector<T> data_;
    
public:
    using value_type = T;
    using size_type = usize;
    using reference = T&;
    using const_reference = const T&;
    
    Stack() = default;
    explicit Stack(size_type reserve_size) { data_.reserve(reserve_size); }
    
    void push(const T& value) { data_.push_back(value); }
    void push(T&& value) { data_.push_back(std::move(value)); }
    
    T pop() {
        if (empty()) throw std::runtime_error("Cannot pop from empty Stack");
        T result = std::move(data_.back());
        data_.pop_back();
        return result;
    }
    
    reference top() {
        if (empty()) throw std::runtime_error("Stack is empty");
        return data_.back();
    }
    
    const_reference top() const {
        if (empty()) throw std::runtime_error("Stack is empty");
        return data_.back();
    }
    
    size_type size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }
    
    void clear() noexcept { data_.clear(); }
    void reserve(size_type capacity) { data_.reserve(capacity); }
};

// Simple queue
template<typename T>
class Queue {
private:
    std::vector<T> data_;
    usize front_index_;
    
public:
    using value_type = T;
    using size_type = usize;
    using reference = T&;
    using const_reference = const T&;
    
    Queue() : front_index_(0) {}
    explicit Queue(size_type reserve_size) : front_index_(0) { 
        data_.reserve(reserve_size); 
    }
    
    void push(const T& value) { data_.push_back(value); }
    void push(T&& value) { data_.push_back(std::move(value)); }
    
    T pop() {
        if (empty()) throw std::runtime_error("Cannot pop from empty Queue");
        T result = std::move(data_[front_index_]);
        ++front_index_;
        
        // Periodic cleanup to free memory
        if (front_index_ > data_.size() / 2) {
            data_.erase(data_.begin(), data_.begin() + front_index_);
            front_index_ = 0;
        }
        
        return result;
    }
    
    reference front() {
        if (empty()) throw std::runtime_error("Queue is empty");
        return data_[front_index_];
    }
    
    const_reference front() const {
        if (empty()) throw std::runtime_error("Queue is empty");
        return data_[front_index_];
    }
    
    reference back() {
        if (empty()) throw std::runtime_error("Queue is empty");
        return data_.back();
    }
    
    const_reference back() const {
        if (empty()) throw std::runtime_error("Queue is empty");
        return data_.back();
    }
    
    size_type size() const noexcept { 
        return data_.size() - front_index_; 
    }
    
    bool empty() const noexcept { 
        return front_index_ >= data_.size(); 
    }
    
    void clear() noexcept { 
        data_.clear(); 
        front_index_ = 0; 
    }
    
    void reserve(size_type capacity) { data_.reserve(capacity); }
};

// Hash map with open addressing (simple implementation)
template<typename Key, typename Value>
class SimpleHashMap {
private:
    struct Entry {
        Key key;
        Value value;
        bool occupied = false;
        bool deleted = false;
    };
    
    std::vector<Entry> buckets_;
    usize size_;
    usize capacity_;
    
    usize hash(const Key& key) const {
        return std::hash<Key>{}(key) % capacity_;
    }
    
    usize find_slot(const Key& key) const {
        usize index = hash(key);
        usize original = index;
        
        while (buckets_[index].occupied && buckets_[index].key != key) {
            index = (index + 1) % capacity_;
            if (index == original) break; // Table is full
        }
        
        return index;
    }
    
    void rehash() {
        auto old_buckets = std::move(buckets_);
        capacity_ *= 2;
        buckets_ = std::vector<Entry>(capacity_);
        size_ = 0;
        
        for (const auto& entry : old_buckets) {
            if (entry.occupied && !entry.deleted) {
                insert(entry.key, entry.value);
            }
        }
    }
    
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using size_type = usize;
    
    explicit SimpleHashMap(size_type initial_capacity = 16) 
        : size_(0), capacity_(initial_capacity) {
        buckets_.resize(capacity_);
    }
    
    void insert(const Key& key, const Value& value) {
        if (size_ >= capacity_ * 0.75) { // Load factor 75%
            rehash();
        }
        
        usize index = find_slot(key);
        if (!buckets_[index].occupied || buckets_[index].deleted) {
            ++size_;
        }
        
        buckets_[index] = {key, value, true, false};
    }
    
    bool contains(const Key& key) const {
        usize index = find_slot(key);
        return buckets_[index].occupied && !buckets_[index].deleted && 
               buckets_[index].key == key;
    }
    
    Value* find(const Key& key) {
        usize index = find_slot(key);
        if (buckets_[index].occupied && !buckets_[index].deleted && 
            buckets_[index].key == key) {
            return &buckets_[index].value;
        }
        return nullptr;
    }
    
    const Value* find(const Key& key) const {
        usize index = find_slot(key);
        if (buckets_[index].occupied && !buckets_[index].deleted && 
            buckets_[index].key == key) {
            return &buckets_[index].value;
        }
        return nullptr;
    }
    
    bool erase(const Key& key) {
        usize index = find_slot(key);
        if (buckets_[index].occupied && !buckets_[index].deleted && 
            buckets_[index].key == key) {
            buckets_[index].deleted = true;
            --size_;
            return true;
        }
        return false;
    }
    
    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    
    void clear() {
        buckets_.clear();
        buckets_.resize(capacity_);
        size_ = 0;
    }
    
    // Operator[]
    Value& operator[](const Key& key) {
        if (size_ >= capacity_ * 0.75) {
            rehash();
        }
        
        usize index = find_slot(key);
        if (!buckets_[index].occupied || buckets_[index].deleted) {
            buckets_[index] = {key, Value{}, true, false};
            ++size_;
        }
        return buckets_[index].value;
    }
};

} // namespace containers
} // namespace badcpplib 