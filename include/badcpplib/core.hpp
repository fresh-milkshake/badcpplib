#pragma once

/**
 * @file core.hpp
 * @brief Core type definitions and fundamental utilities
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides fundamental type aliases and smart pointer utilities
 * that form the foundation of BadCppLib. It is automatically included
 * in all builds and cannot be disabled.
 */

#include <cstdint>
#include <memory>
#include <string>

namespace badcpplib {

/**
 * @namespace badcpplib::core
 * @brief Core type definitions and fundamental utilities
 * 
 * This namespace provides type aliases for common C++ types with shorter,
 * more convenient names inspired by Rust's type system. All types are
 * re-exported into the main badcpplib namespace for convenience.
 * 
 * @section type_categories Type Categories
 * 
 * - **Integer Types**: Fixed-width signed and unsigned integers
 * - **Floating-Point Types**: IEEE 754 floating-point numbers
 * - **Size Types**: Platform-specific size and pointer difference types
 * - **String Types**: Standard string type alias
 * - **Smart Pointers**: Modern C++ memory management utilities
 * 
 * @example{lineno}
 * ```cpp
 * #include "badcpplib/core.hpp"
 * using namespace badcpplib;
 * 
 * // Use short type names
 * i32 integer = 42;
 * f64 pi = 3.14159;
 * string greeting = "Hello, BadCppLib!";
 * 
 * // Smart pointers
 * auto ptr = make_unique<string>("Dynamic string");
 * auto shared = make_shared<i32>(100);
 * ```
 */
namespace core {

/**
 * @defgroup integer_types Integer Types
 * @brief Fixed-width integer type aliases
 * 
 * These type aliases provide consistent, fixed-width integer types
 * across all platforms. They follow Rust naming conventions for brevity.
 * 
 * @{
 */

/// @brief 8-bit signed integer (-128 to 127)
using i8 = std::int8_t;

/// @brief 16-bit signed integer (-32,768 to 32,767)
using i16 = std::int16_t;

/// @brief 32-bit signed integer (-2,147,483,648 to 2,147,483,647)
using i32 = std::int32_t;

/// @brief 64-bit signed integer (-9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)
using i64 = std::int64_t;

/// @brief 128-bit signed integer (compiler-dependent availability)
#if defined(__GNUC__) || defined(__clang__)
using i128 = __int128;
#endif

/// @brief 8-bit unsigned integer (0 to 255)
using u8 = std::uint8_t;

/// @brief 16-bit unsigned integer (0 to 65,535)
using u16 = std::uint16_t;

/// @brief 32-bit unsigned integer (0 to 4,294,967,295)
using u32 = std::uint32_t;

/// @brief 64-bit unsigned integer (0 to 18,446,744,073,709,551,615)
using u64 = std::uint64_t;

/// @brief 128-bit unsigned integer (compiler-dependent availability)
#if defined(__GNUC__) || defined(__clang__)
using u128 = unsigned __int128;
#endif

/** @} */

/**
 * @defgroup floating_types Floating-Point Types
 * @brief IEEE 754 floating-point type aliases
 * 
 * These aliases provide consistent floating-point types with
 * explicit precision indicators.
 * 
 * @{
 */

/// @brief 32-bit IEEE 754 single-precision floating-point (±3.4e±38, ~7 decimal digits)
using f32 = float;

/// @brief 64-bit IEEE 754 double-precision floating-point (±1.7e±308, ~15 decimal digits)
using f64 = double;

/// @brief Extended precision floating-point (implementation-defined, typically 80 or 128 bits)
using f128 = long double;

/** @} */

/**
 * @defgroup size_types Size and Pointer Types
 * @brief Platform-specific size and pointer difference types
 * 
 * These types represent sizes and pointer arithmetic results,
 * automatically adjusting to the target platform.
 * 
 * @{
 */

/// @brief Unsigned integer type for representing object sizes and array indices
using usize = std::size_t;

/// @brief Signed integer type for representing pointer differences and signed sizes
using isize = std::ptrdiff_t;

/** @} */

/**
 * @defgroup string_types String Types
 * @brief String type aliases
 * @{
 */

/// @brief Standard string type alias for convenience
using string = std::string;

/** @} */

/**
 * @defgroup smart_pointers Smart Pointer Types and Utilities
 * @brief Modern C++ memory management utilities
 * 
 * Smart pointers provide automatic memory management and help prevent
 * memory leaks and dangling pointers. These aliases and utility functions
 * make smart pointer usage more convenient.
 * 
 * @{
 */

/**
 * @brief Alias for std::unique_ptr with automatic type deduction
 * @tparam T The type being managed by the unique_ptr
 * 
 * Provides exclusive ownership of a dynamically allocated object.
 * Cannot be copied, only moved.
 * 
 * @usecase{RAII resource management}
 * Use for managing resources that should have single ownership semantics.
 * 
 * @example
 * ```cpp
 * auto ptr = make_unique<string>("Hello");
 * // ptr is automatically cleaned up when it goes out of scope
 * ```
 */
template<typename T>
using unique_ptr = std::unique_ptr<T>;

/**
 * @brief Alias for std::shared_ptr with automatic type deduction
 * @tparam T The type being managed by the shared_ptr
 * 
 * Provides shared ownership of a dynamically allocated object.
 * Multiple shared_ptr instances can point to the same object.
 * 
 * @usecase{Shared resource management}
 * Use when multiple objects need to share ownership of a resource.
 * 
 * @example
 * ```cpp
 * auto shared1 = make_shared<i32>(42);
 * auto shared2 = shared1; // Both point to the same integer
 * ```
 */
template<typename T>
using shared_ptr = std::shared_ptr<T>;

/**
 * @brief Alias for std::weak_ptr with automatic type deduction
 * @tparam T The type being managed by the weak_ptr
 * 
 * Provides non-owning weak reference to an object managed by shared_ptr.
 * Helps break circular references.
 * 
 * @usecase{Breaking circular references}
 * Use to observe shared_ptr managed objects without extending their lifetime.
 */
template<typename T>
using weak_ptr = std::weak_ptr<T>;

/**
 * @brief Create a unique_ptr with perfect forwarding
 * @tparam T The type to create
 * @tparam Args Constructor argument types (deduced)
 * @param args Constructor arguments
 * @return unique_ptr<T> managing a new instance of T
 * 
 * Creates a new object of type T and wraps it in a unique_ptr.
 * All arguments are perfectly forwarded to T's constructor.
 * 
 * @complexity{O(1)} plus T's constructor complexity
 * @threadsafe This function is thread-safe
 * @since{1.0.0}
 * 
 * @usecase{Safe dynamic allocation}
 * Use instead of raw `new` to ensure automatic memory management.
 * 
 * @example
 * ```cpp
 * // Create unique_ptr to string
 * auto str_ptr = make_unique<string>("Hello, World!");
 * 
 * // Create unique_ptr to vector
 * auto vec_ptr = make_unique<vector<i32>>(10, 42); // 10 elements, all 42
 * 
 * // Create unique_ptr to custom object
 * struct Point { i32 x, y; Point(i32 x, i32 y) : x(x), y(y) {} };
 * auto point = make_unique<Point>(10, 20);
 * ```
 */
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

/**
 * @brief Create a shared_ptr with perfect forwarding
 * @tparam T The type to create
 * @tparam Args Constructor argument types (deduced)
 * @param args Constructor arguments
 * @return shared_ptr<T> managing a new instance of T
 * 
 * Creates a new object of type T and wraps it in a shared_ptr.
 * All arguments are perfectly forwarded to T's constructor.
 * More efficient than creating shared_ptr from unique_ptr.
 * 
 * @complexity{O(1)} plus T's constructor complexity
 * @threadsafe This function is thread-safe
 * @since{1.0.0}
 * 
 * @usecase{Shared object creation}
 * Use when multiple objects need to share ownership of a resource.
 * 
 * @example
 * ```cpp
 * // Create shared_ptr to string
 * auto str_ptr = make_shared<string>("Shared string");
 * auto str_ptr2 = str_ptr; // Share ownership
 * 
 * // Create shared_ptr to vector
 * auto vec_ptr = make_shared<vector<f64>>(100, 3.14);
 * 
 * // Pass to multiple functions that need shared access
 * process_data(vec_ptr);
 * analyze_data(vec_ptr);
 * ```
 */
template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/** @} */

} // namespace core

// Export types to main namespace for compatibility
using namespace core;

} // namespace badcpplib 