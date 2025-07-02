#pragma once

/**
 * @file result.hpp
 * @brief Rust-style Result type for safe error handling without exceptions
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides a Result<T, E> type inspired by Rust's Result enum,
 * allowing for explicit error handling without exceptions. Enable with
 * `BADCPPLIB_ENABLE_RESULT`.
 */

#include "core.hpp"
#include <variant>
#include <stdexcept>

namespace badcpplib {

/**
 * @namespace badcpplib::result
 * @brief Rust-style Result type for safe error handling
 * 
 * This namespace provides a Result<T, E> type that represents either a
 * successful value of type T or an error of type E. This approach allows
 * for explicit error handling without exceptions, leading to more
 * predictable and safer code.
 * 
 * @section philosophy Design Philosophy
 * 
 * The Result type forces developers to explicitly handle error cases,
 * making error handling visible in the code and preventing forgotten
 * error checks. This leads to more robust applications.
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_RESULT
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * // Function that might fail
 * Result<i32, string> divide(i32 a, i32 b) {
 *     if (b == 0) {
 *         return Result<i32, string>::err("Division by zero");
 *     }
 *     return Result<i32, string>::ok(a / b);
 * }
 * 
 * // Usage with explicit error handling
 * auto result = divide(10, 2);
 * if (result.is_ok()) {
 *     i32 value = result.unwrap();
 *     // Use value
 * } else {
 *     string error = result.error();
 *     // Handle error
 * }
 * ```
 */
namespace result {

/**
 * @brief Error wrapper to distinguish error types in variant
 * @tparam E The error type
 * 
 * This wrapper is used internally by Result to distinguish between
 * success values and error values when stored in a variant.
 * 
 * @since{1.0.0}
 */
template<typename E>
struct ErrorWrapper {
    E error; ///< The wrapped error value
    
    /**
     * @brief Construct from error value (copy)
     * @param e The error to wrap
     */
    ErrorWrapper(const E& e) : error(e) {}
    
    /**
     * @brief Construct from error value (move)
     * @param e The error to wrap
     */
    ErrorWrapper(E&& e) : error(std::move(e)) {}
    
    /**
     * @brief Get the wrapped error (const)
     * @return const E& Reference to the error
     */
    const E& get() const { return error; }
    
    /**
     * @brief Get the wrapped error (mutable)
     * @return E& Reference to the error
     */
    E& get() { return error; }
};

/**
 * @brief Result type for safe error handling without exceptions
 * @tparam T The success value type
 * @tparam E The error type (defaults to string)
 * 
 * A Result<T, E> represents either success (containing a value of type T)
 * or failure (containing an error of type E). This type forces explicit
 * error handling and eliminates the possibility of forgetting to check
 * for errors.
 * 
 * @section lifecycle Object Lifecycle
 * Results are copyable and movable, and automatically manage the lifetime
 * of their contained values.
 * 
 * @section thread_safety Thread Safety
 * Result objects are not thread-safe. If sharing between threads is needed,
 * external synchronization is required.
 * 
 * @usecase{API functions that might fail}
 * Use Result for functions that can fail, such as file operations,
 * network requests, or parsing operations.
 * 
 * @usecase{Avoiding exceptions}
 * In performance-critical code where exception overhead is unacceptable,
 * use Result for error handling.
 * 
 * @example
 * ```cpp
 * Result<i32, string> parse_int(const string& str) {
 *     try {
 *         return Result<i32, string>::ok(std::stoi(str));
 *     } catch (const std::exception& e) {
 *         return Result<i32, string>::err(e.what());
 *     }
 * }
 * 
 * auto result = parse_int("42");
 * i32 value = result.unwrap_or(0);  // Gets 42 or 0 if error
 * ```
 * 
 * @since{1.0.0}
 */
template<typename T, typename E = string>
class Result {
private:
    std::variant<T, ErrorWrapper<E>> data_; ///< Internal storage for value or error

public:
    /**
     * @defgroup result_constructors Constructors
     * @brief Construction methods for Result objects
     * @{
     */
    
    /**
     * @brief Construct a successful Result from a value (copy)
     * @param value The success value
     */
    Result(const T& value) : data_(value) {}
    
    /**
     * @brief Construct a successful Result from a value (move)
     * @param value The success value
     */
    Result(T&& value) : data_(std::move(value)) {}
    
    /**
     * @brief Construct an error Result from ErrorWrapper (copy)
     * @param error The error wrapper
     */
    Result(const ErrorWrapper<E>& error) : data_(error) {}
    
    /**
     * @brief Construct an error Result from ErrorWrapper (move)
     * @param error The error wrapper
     */
    Result(ErrorWrapper<E>&& error) : data_(std::move(error)) {}

    /** @} */

    /**
     * @defgroup result_factory Static Factory Methods
     * @brief Static methods for creating Result instances
     * @{
     */

    /**
     * @brief Create a successful Result (copy)
     * @param value The success value
     * @return Result<T, E> A Result containing the value
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto success = Result<i32, string>::ok(42);
     * ```
     */
    static Result<T, E> ok(const T& value) {
        return Result<T, E>(value);
    }

    /**
     * @brief Create a successful Result (move)
     * @param value The success value
     * @return Result<T, E> A Result containing the value
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto success = Result<i32, string>::ok(std::move(expensive_object));
     * ```
     */
    static Result<T, E> ok(T&& value) {
        return Result<T, E>(std::move(value));
    }

    /**
     * @brief Create an error Result (copy)
     * @param error The error value
     * @return Result<T, E> A Result containing the error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto failure = Result<i32, string>::err("Something went wrong");
     * ```
     */
    static Result<T, E> err(const E& error) {
        return Result<T, E>(ErrorWrapper<E>(error));
    }

    /**
     * @brief Create an error Result (move)
     * @param error The error value
     * @return Result<T, E> A Result containing the error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto failure = Result<i32, string>::err(std::move(error_message));
     * ```
     */
    static Result<T, E> err(E&& error) {
        return Result<T, E>(ErrorWrapper<E>(std::move(error)));
    }

    /** @} */

    /**
     * @defgroup result_query State Query Methods
     * @brief Methods for checking the state of a Result
     * @{
     */

    /**
     * @brief Check if Result contains a success value
     * @return bool True if Result contains a value, false if it contains an error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto result = divide(10, 2);
     * if (result.is_ok()) {
     *     // Handle success case
     * }
     * ```
     */
    bool is_ok() const {
        return std::holds_alternative<T>(data_);
    }

    /**
     * @brief Check if Result contains an error
     * @return bool True if Result contains an error, false if it contains a value
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto result = divide(10, 0);
     * if (result.is_err()) {
     *     // Handle error case
     * }
     * ```
     */
    bool is_err() const {
        return std::holds_alternative<ErrorWrapper<E>>(data_);
    }

    /** @} */

    /**
     * @defgroup result_access Value Access Methods
     * @brief Methods for accessing contained values (potentially unsafe)
     * @{
     */

    /**
     * @brief Get the contained value (const version)
     * @return const T& Reference to the success value
     * @throws std::runtime_error If Result contains an error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @warning This method throws if called on an error Result.
     *          Use is_ok() to check first, or use unwrap_or() for safety.
     * 
     * @usecase{When you're certain the Result is Ok}
     * Use this when you've already checked is_ok() or when failure
     * would be a programming error.
     * 
     * @example
     * ```cpp
     * auto result = Result<i32, string>::ok(42);
     * if (result.is_ok()) {
     *     i32 value = result.unwrap();  // Safe because we checked
     * }
     * ```
     */
    const T& unwrap() const {
        if (is_err()) {
            throw std::runtime_error("Called unwrap on an error Result");
        }
        return std::get<T>(data_);
    }

    /**
     * @brief Get the contained value (mutable version)
     * @return T& Reference to the success value
     * @throws std::runtime_error If Result contains an error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @warning This method throws if called on an error Result.
     *          Use is_ok() to check first.
     * 
     * @example
     * ```cpp
     * auto result = Result<string, string>::ok("hello");
     * if (result.is_ok()) {
     *     result.unwrap() += " world";  // Modify the contained value
     * }
     * ```
     */
    T& unwrap() {
        if (is_err()) {
            throw std::runtime_error("Called unwrap on an error Result");
        }
        return std::get<T>(data_);
    }

    /**
     * @brief Get the contained value with custom panic message (const)
     * @param message Custom error message to use if Result contains an error
     * @return const T& Reference to the success value
     * @throws std::runtime_error With the custom message if Result contains an error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @usecase{Better error reporting}
     * Use this when you want to provide context-specific error messages
     * for debugging.
     * 
     * @example
     * ```cpp
     * auto config = load_config();
     * string host = config.expect("Failed to load server configuration").host;
     * ```
     */
    const T& expect(const string& message) const {
        if (is_err()) {
            throw std::runtime_error(message);
        }
        return std::get<T>(data_);
    }

    /**
     * @brief Get the contained value with custom panic message (mutable)
     * @param message Custom error message to use if Result contains an error
     * @return T& Reference to the success value
     * @throws std::runtime_error With the custom message if Result contains an error
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @example
     * ```cpp
     * auto result = parse_data();
     * result.expect("Critical: Failed to parse input data").process();
     * ```
     */
    T& expect(const string& message) {
        if (is_err()) {
            throw std::runtime_error(message);
        }
        return std::get<T>(data_);
    }

    /** @} */

    /**
     * @defgroup result_error Error Access Methods
     * @brief Methods for accessing contained errors
     * @{
     */

    /**
     * @brief Get the contained error (const version)
     * @return const E& Reference to the error value
     * @throws std::runtime_error If Result contains a success value
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @warning This method throws if called on a success Result.
     *          Use is_err() to check first.
     * 
     * @example
     * ```cpp
     * auto result = Result<i32, string>::err("error message");
     * if (result.is_err()) {
     *     string error_msg = result.error();  // Safe because we checked
     * }
     * ```
     */
    const E& error() const {
        if (is_ok()) {
            throw std::runtime_error("Called error on an ok Result");
        }
        return std::get<ErrorWrapper<E>>(data_).get();
    }

    /**
     * @brief Get the contained error (mutable version)
     * @return E& Reference to the error value
     * @throws std::runtime_error If Result contains a success value
     * 
     * @complexity{O(1)}
     * @since{1.0.0}
     * 
     * @warning This method throws if called on a success Result.
     *          Use is_err() to check first.
     */
    E& error() {
        if (is_ok()) {
            throw std::runtime_error("Called error on an ok Result");
        }
        return std::get<ErrorWrapper<E>>(data_).get();
    }

    /** @} */

    // Get value or default value
    T unwrap_or(const T& default_value) const {
        if (is_ok()) {
            return std::get<T>(data_);
        }
        return default_value;
    }

    T unwrap_or(T&& default_value) const {
        if (is_ok()) {
            return std::get<T>(data_);
        }
        return std::move(default_value);
    }

    // Apply function if value exists
    template<typename F>
    auto map(F&& func) const -> Result<decltype(func(std::get<T>(data_))), E> {
        if (is_ok()) {
            return Result<decltype(func(std::get<T>(data_))), E>::ok(func(std::get<T>(data_)));
        }
        return Result<decltype(func(std::get<T>(data_))), E>::err(std::get<ErrorWrapper<E>>(data_).get());
    }

    // Apply function to error
    template<typename F>
    auto map_err(F&& func) const -> Result<T, decltype(func(std::get<ErrorWrapper<E>>(data_).get()))> {
        if (is_err()) {
            return Result<T, decltype(func(std::get<ErrorWrapper<E>>(data_).get()))>::err(
                func(std::get<ErrorWrapper<E>>(data_).get())
            );
        }
        return Result<T, decltype(func(std::get<ErrorWrapper<E>>(data_).get()))>::ok(std::get<T>(data_));
    }

    // Operation chain
    template<typename F>
    auto and_then(F&& func) const -> decltype(func(std::get<T>(data_))) {
        if (is_ok()) {
            return func(std::get<T>(data_));
        }
        return decltype(func(std::get<T>(data_)))::err(std::get<ErrorWrapper<E>>(data_).get());
    }
};

// Specialization for void
template<typename E>
class Result<void, E> {
private:
    std::variant<std::monostate, ErrorWrapper<E>> data_;

public:
    Result() : data_(std::monostate{}) {}
    Result(const ErrorWrapper<E>& error) : data_(error) {}
    Result(ErrorWrapper<E>&& error) : data_(std::move(error)) {}

    static Result<void, E> ok() {
        return Result<void, E>();
    }

    static Result<void, E> err(const E& error) {
        return Result<void, E>(ErrorWrapper<E>(error));
    }

    static Result<void, E> err(E&& error) {
        return Result<void, E>(ErrorWrapper<E>(std::move(error)));
    }

    bool is_ok() const {
        return std::holds_alternative<std::monostate>(data_);
    }

    bool is_err() const {
        return std::holds_alternative<ErrorWrapper<E>>(data_);
    }

    void unwrap() const {
        if (is_err()) {
            throw std::runtime_error("Called unwrap on an error Result");
        }
    }

    void expect(const string& message) const {
        if (is_err()) {
            throw std::runtime_error(message);
        }
    }

    const E& error() const {
        if (is_ok()) {
            throw std::runtime_error("Called error on an ok Result");
        }
        return std::get<ErrorWrapper<E>>(data_).get();
    }

    E& error() {
        if (is_ok()) {
            throw std::runtime_error("Called error on an ok Result");
        }
        return std::get<ErrorWrapper<E>>(data_).get();
    }
};

} // namespace result

// Export to main namespace
using namespace result;

} // namespace badcpplib 