#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <optional>
#include <variant>
#include <type_traits>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <random>
#include <thread>
#include <iostream>
#include <numeric>

namespace badcpplib
{
    // ===== BASIC TYPES =====
    namespace types {
        using i8 = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;
#if defined(__GNUC__) || defined(__clang__)
        using i128 = __int128_t;
#endif

        using u8 = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;
#if defined(__GNUC__) || defined(__clang__)
        using u128 = __uint128_t;
#endif

        using f32 = float;
        using f64 = double;
        using f128 = long double;

        using usize = std::size_t;
        using isize = std::ptrdiff_t;

        template<typename T>
        using unique_ptr = std::unique_ptr<T>;
        
        template<typename T>
        using shared_ptr = std::shared_ptr<T>;
        
        template<typename T>
        using weak_ptr = std::weak_ptr<T>;

        template<typename T>
        using optional = std::optional<T>;

        template<typename... Types>
        using variant = std::variant<Types...>;

        using string = std::string;
        using string_view = std::string_view;

        template<typename T>
        using vector = std::vector<T>;

        template<typename K, typename V>
        using map = std::unordered_map<K, V>;
    }

    using namespace types;

    // ===== RESULT TYPE (Rust-style error handling) =====
    
    // Error wrapper to distinguish types in variant
    template<typename E>
    struct ErrorWrapper {
        E error;
        
        ErrorWrapper(E&& e);
        ErrorWrapper(const E& e);
    };
    
    template<typename T, typename E = string>
    class Result {
    private:
        variant<T, ErrorWrapper<E>> data_;

    public:
        // Static methods for creation
        static Result ok(T&& value);
        static Result ok(const T& value);
        static Result err(E&& error);
        static Result err(const E& error);

    private:
        Result(T&& value);
        Result(const T& value);
        Result(ErrorWrapper<E>&& wrapper);

    public:

        bool is_ok() const;
        bool is_err() const;

        T& unwrap();
        const T& unwrap() const;
        T unwrap_or(T&& default_value);

        E& error();
        const E& error() const;

        template<typename F>
        auto map(F&& func) -> Result<decltype(func(std::get<T>(data_))), E>;
    };

    // ===== STRING UTILITIES =====
    namespace string_utils {
        vector<string> split(const string& str, char delimiter);
        string join(const vector<string>& strings, const string& delimiter);
        string trim(const string& str);
        string to_lower(const string& str);
        string to_upper(const string& str);
        bool starts_with(const string& str, const string& prefix);
        bool ends_with(const string& str, const string& suffix);
        bool contains(const string& str, const string& substring);
    }

    // ===== FILE OPERATIONS =====
    namespace file_utils {
        Result<string> read_file(const string& filepath);
        Result<bool> write_file(const string& filepath, const string& content);
        bool file_exists(const string& filepath);
    }

    // ===== MATHEMATICAL UTILITIES =====
    namespace math_utils {
        template<typename T>
        T clamp(T value, T min_val, T max_val);

        template<typename T>
        T lerp(T a, T b, f64 t);

        template<typename T>
        bool is_power_of_two(T value);

        template<typename T>
        T next_power_of_two(T value);

        f64 random_double(f64 min = 0.0, f64 max = 1.0);
        i32 random_int(i32 min, i32 max);
    }

    // ===== TIME MEASUREMENT =====
    namespace time_utils {
        class Timer {
        private:
            std::chrono::high_resolution_clock::time_point start_;

        public:
            Timer();
            void reset();
            f64 elapsed_seconds() const;
            i64 elapsed_milliseconds() const;
            i64 elapsed_microseconds() const;
        };

        template<typename F>
        f64 measure_time(F&& func);

        void sleep_ms(i32 milliseconds);
    }

    // ===== CONTAINERS AND ALGORITHMS =====
    namespace containers {
        template<typename T, usize N>
        class StaticArray {
        private:
            T data_[N];
            usize size_;

        public:
            StaticArray();
            void push(const T& item);
            void pop();
            T& operator[](usize index);
            const T& operator[](usize index) const;
            usize size() const;
            usize capacity() const;
            bool empty() const;
            bool full() const;
            T* begin();
            T* end();
            const T* begin() const;
            const T* end() const;
        };

        template<typename T>
        class CircularBuffer {
        private:
            vector<T> buffer_;
            usize head_;
            usize tail_;
            usize size_;
            usize capacity_;

        public:
            CircularBuffer(usize capacity);
            void push(const T& item);
            optional<T> pop();
            usize size() const;
            bool empty() const;
            bool full() const;
        };
    }

    // ===== FUNCTIONAL PROGRAMMING =====
    namespace functional {
        template<typename Container, typename Predicate>
        auto filter(const Container& container, Predicate pred);

        template<typename Container, typename Func>
        auto map(const Container& container, Func func);

        template<typename Container, typename T, typename Func>
        T reduce(const Container& container, T initial, Func func);

        template<typename Container, typename Predicate>
        bool any(const Container& container, Predicate pred);

        template<typename Container, typename Predicate>
        bool all(const Container& container, Predicate pred);
    }

    // ===== DEBUG UTILITIES =====
    namespace debug {
        template<typename T>
        void print(const T& value);

        template<typename T, typename... Args>
        void print(const T& first, const Args&... args);

        template<typename Container>
        void print_container(const Container& container);

        void assert_true(bool condition, const string& message = "Assertion failed");
    }

    // ===== MEMORY AND RESOURCES =====
    namespace memory {
        template<typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args);

        template<typename T, typename... Args>
        shared_ptr<T> make_shared(Args&&... args);

        class ScopeGuard {
        private:
            std::function<void()> cleanup_;

        public:
            ScopeGuard(std::function<void()> cleanup);
            ~ScopeGuard();
            
            ScopeGuard(const ScopeGuard&) = delete;
            ScopeGuard& operator=(const ScopeGuard&) = delete;
        };

        #define SCOPE_EXIT(code) \
            auto scope_guard_##__LINE__ = memory::ScopeGuard([&]() { code; })
    }

    // ===== TEMPLATE FUNCTION IMPLEMENTATIONS =====

    // ErrorWrapper implementation
    template<typename E>
    ErrorWrapper<E>::ErrorWrapper(E&& e) : error(std::forward<E>(e)) {}
    
    template<typename E>
    ErrorWrapper<E>::ErrorWrapper(const E& e) : error(e) {}

    // Result class implementation
    template<typename T, typename E>
    Result<T, E> Result<T, E>::ok(T&& value) {
        return Result(std::forward<T>(value));
    }

    template<typename T, typename E>
    Result<T, E> Result<T, E>::ok(const T& value) {
        return Result(value);
    }

    template<typename T, typename E>
    Result<T, E> Result<T, E>::err(E&& error) {
        return Result(ErrorWrapper<E>(std::forward<E>(error)));
    }

    template<typename T, typename E>
    Result<T, E> Result<T, E>::err(const E& error) {
        return Result(ErrorWrapper<E>(error));
    }

    template<typename T, typename E>
    Result<T, E>::Result(T&& value) : data_(std::forward<T>(value)) {}

    template<typename T, typename E>
    Result<T, E>::Result(const T& value) : data_(value) {}

    template<typename T, typename E>
    Result<T, E>::Result(ErrorWrapper<E>&& wrapper) : data_(std::move(wrapper)) {}

    template<typename T, typename E>
    bool Result<T, E>::is_ok() const { 
        return std::holds_alternative<T>(data_); 
    }

    template<typename T, typename E>
    bool Result<T, E>::is_err() const { 
        return std::holds_alternative<ErrorWrapper<E>>(data_); 
    }

    template<typename T, typename E>
    T& Result<T, E>::unwrap() {
        if (is_err()) throw std::runtime_error("Called unwrap on an Err value");
        return std::get<T>(data_);
    }

    template<typename T, typename E>
    const T& Result<T, E>::unwrap() const {
        if (is_err()) throw std::runtime_error("Called unwrap on an Err value");
        return std::get<T>(data_);
    }

    template<typename T, typename E>
    T Result<T, E>::unwrap_or(T&& default_value) {
        return is_ok() ? std::get<T>(data_) : default_value;
    }

    template<typename T, typename E>
    E& Result<T, E>::error() {
        if (is_ok()) throw std::runtime_error("Called error on an Ok value");
        return std::get<ErrorWrapper<E>>(data_).error;
    }

    template<typename T, typename E>
    const E& Result<T, E>::error() const {
        if (is_ok()) throw std::runtime_error("Called error on an Ok value");
        return std::get<ErrorWrapper<E>>(data_).error;
    }

    template<typename T, typename E>
    template<typename F>
    auto Result<T, E>::map(F&& func) -> Result<decltype(func(std::get<T>(data_))), E> {
        if (is_ok()) {
            return Result<decltype(func(std::get<T>(data_))), E>::ok(func(std::get<T>(data_)));
        }
        return Result<decltype(func(std::get<T>(data_))), E>::err(error());
    }

    // Math utils template implementations
    template<typename T>
    T math_utils::clamp(T value, T min_val, T max_val) {
        return std::max(min_val, std::min(value, max_val));
    }

    template<typename T>
    T math_utils::lerp(T a, T b, f64 t) {
        return a + static_cast<T>((b - a) * t);
    }

    template<typename T>
    bool math_utils::is_power_of_two(T value) {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template<typename T>
    T math_utils::next_power_of_two(T value) {
        if (value <= 1) return 2;
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        if constexpr (sizeof(T) > 4) {
            value |= value >> 32;
        }
        return value + 1;
    }

    // Time utils implementations
    inline time_utils::Timer::Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    inline void time_utils::Timer::reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }

    inline f64 time_utils::Timer::elapsed_seconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        return duration.count() / 1000000.0;
    }

    inline i64 time_utils::Timer::elapsed_milliseconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
        return duration.count();
    }

    inline i64 time_utils::Timer::elapsed_microseconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        return duration.count();
    }

    inline void time_utils::sleep_ms(i32 milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    // Time utils template implementations
    template<typename F>
    f64 time_utils::measure_time(F&& func) {
        Timer timer;
        func();
        return timer.elapsed_seconds();
    }

    // Container template implementations
    template<typename T, usize N>
    containers::StaticArray<T, N>::StaticArray() : size_(0) {}

    template<typename T, usize N>
    void containers::StaticArray<T, N>::push(const T& item) {
        if (size_ < N) {
            data_[size_++] = item;
        }
    }

    template<typename T, usize N>
    void containers::StaticArray<T, N>::pop() {
        if (size_ > 0) {
            size_--;
        }
    }

    template<typename T, usize N>
    T& containers::StaticArray<T, N>::operator[](usize index) {
        return data_[index];
    }

    template<typename T, usize N>
    const T& containers::StaticArray<T, N>::operator[](usize index) const {
        return data_[index];
    }

    template<typename T, usize N>
    usize containers::StaticArray<T, N>::size() const { return size_; }

    template<typename T, usize N>
    usize containers::StaticArray<T, N>::capacity() const { return N; }

    template<typename T, usize N>
    bool containers::StaticArray<T, N>::empty() const { return size_ == 0; }

    template<typename T, usize N>
    bool containers::StaticArray<T, N>::full() const { return size_ == N; }

    template<typename T, usize N>
    T* containers::StaticArray<T, N>::begin() { return data_; }

    template<typename T, usize N>
    T* containers::StaticArray<T, N>::end() { return data_ + size_; }

    template<typename T, usize N>
    const T* containers::StaticArray<T, N>::begin() const { return data_; }

    template<typename T, usize N>
    const T* containers::StaticArray<T, N>::end() const { return data_ + size_; }

    template<typename T>
    containers::CircularBuffer<T>::CircularBuffer(usize capacity) 
        : buffer_(capacity), head_(0), tail_(0), size_(0), capacity_(capacity) {}

    template<typename T>
    void containers::CircularBuffer<T>::push(const T& item) {
        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;
        if (size_ < capacity_) {
            size_++;
        } else {
            head_ = (head_ + 1) % capacity_;
        }
    }

    template<typename T>
    optional<T> containers::CircularBuffer<T>::pop() {
        if (size_ == 0) return std::nullopt;
        T item = buffer_[head_];
        head_ = (head_ + 1) % capacity_;
        size_--;
        return item;
    }

    template<typename T>
    usize containers::CircularBuffer<T>::size() const { return size_; }

    template<typename T>
    bool containers::CircularBuffer<T>::empty() const { return size_ == 0; }

    template<typename T>
    bool containers::CircularBuffer<T>::full() const { return size_ == capacity_; }

    // Functional programming template implementations
    template<typename Container, typename Predicate>
    auto functional::filter(const Container& container, Predicate pred) {
        Container result;
        std::copy_if(container.begin(), container.end(), 
                    std::back_inserter(result), pred);
        return result;
    }

    template<typename Container, typename Func>
    auto functional::map(const Container& container, Func func) {
        using ValueType = decltype(func(*container.begin()));
        std::vector<ValueType> result;
        std::transform(container.begin(), container.end(), 
                      std::back_inserter(result), func);
        return result;
    }

    template<typename Container, typename T, typename Func>
    T functional::reduce(const Container& container, T initial, Func func) {
        return std::accumulate(container.begin(), container.end(), initial, func);
    }

    template<typename Container, typename Predicate>
    bool functional::any(const Container& container, Predicate pred) {
        return std::any_of(container.begin(), container.end(), pred);
    }

    template<typename Container, typename Predicate>
    bool functional::all(const Container& container, Predicate pred) {
        return std::all_of(container.begin(), container.end(), pred);
    }

    // Debug template implementations
    template<typename T>
    void debug::print(const T& value) {
        std::cout << value << std::endl;
    }

    template<typename T, typename... Args>
    void debug::print(const T& first, const Args&... args) {
        std::cout << first << " ";
        print(args...);
    }

    template<typename Container>
    void debug::print_container(const Container& container) {
        std::cout << "[";
        bool first = true;
        for (const auto& item : container) {
            if (!first) std::cout << ", ";
            std::cout << item;
            first = false;
        }
        std::cout << "]" << std::endl;
    }

    // Memory implementations
    inline memory::ScopeGuard::ScopeGuard(std::function<void()> cleanup) : cleanup_(cleanup) {}
    
    inline memory::ScopeGuard::~ScopeGuard() { 
        cleanup_(); 
    }

    // Memory template implementations
    template<typename T, typename... Args>
    unique_ptr<T> memory::make_unique(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    shared_ptr<T> memory::make_shared(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
} 