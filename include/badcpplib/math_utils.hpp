#pragma once

/**
 * @file math_utils.hpp
 * @brief Mathematical utilities and random number generation
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides mathematical utility functions, random number generation,
 * and mathematical constants. Enable with `BADCPPLIB_ENABLE_MATH`.
 */

#include "core.hpp"
#include <cmath>
#include <random>
#include <thread>
#include <chrono>

namespace badcpplib {

/**
 * @namespace badcpplib::math_utils
 * @brief Mathematical utilities and random number generation
 * 
 * This namespace provides a comprehensive set of mathematical functions
 * including comparison functions, interpolation, random number generation,
 * and mathematical constants commonly used in C++ applications.
 * 
 * @section categories Function Categories
 * 
 * - **Comparison Functions**: min, max, clamp, abs
 * - **Interpolation**: Linear interpolation (lerp)
 * - **Power Functions**: Power-of-two utilities
 * - **Rounding**: Precision rounding functions
 * - **Random Numbers**: Thread-safe random number generation
 * - **Mathematical Constants**: Common mathematical constants
 * - **Utility Functions**: Sleep and timing utilities
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_MATH
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * // Clamp values to range
 * i32 clamped = math_utils::clamp(150, 0, 100);  // 100
 * 
 * // Linear interpolation
 * f64 mid = math_utils::lerp(0.0, 100.0, 0.5);   // 50.0
 * 
 * // Random numbers
 * i32 dice = math_utils::random_int(1, 6);        // 1-6
 * 
 * // Mathematical constants
 * f64 circle_area = math_utils::PI * radius * radius;
 * ```
 */
namespace math_utils {

/**
 * @defgroup comparison_functions Comparison and Clamping Functions
 * @brief Template functions for comparing and constraining values
 * @{
 */

/**
 * @brief Clamp a value between minimum and maximum bounds
 * @tparam T Numeric type (must support comparison operators)
 * @param value The value to clamp
 * @param min The minimum allowed value
 * @param max The maximum allowed value
 * @return T The clamped value
 * 
 * If value < min, returns min. If value > max, returns max.
 * Otherwise returns the original value.
 * 
 * @complexity{O(1)}
 * @since{1.0.0}
 * 
 * @usecase{Input validation}
 * Ensure user input stays within valid ranges.
 * 
 * @usecase{Graphics programming}
 * Clamp color values or coordinates to valid ranges.
 * 
 * @example
 * ```cpp
 * i32 health = math_utils::clamp(150, 0, 100);    // 100
 * f64 opacity = math_utils::clamp(-0.5, 0.0, 1.0); // 0.0
 * u8 color = math_utils::clamp(300, 0, 255);       // 255
 * ```
 */
template<typename T>
constexpr T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

/**
 * @brief Get the minimum of two values
 * @tparam T Numeric type (must support comparison operators)
 * @param a First value
 * @param b Second value
 * @return T The smaller value
 * 
 * @complexity{O(1)}
 * @since{1.0.0}
 * 
 * @example
 * ```cpp
 * i32 smaller = math_utils::min(10, 20);  // 10
 * f64 min_val = math_utils::min(3.14, 2.71); // 2.71
 * ```
 */
template<typename T>
constexpr T min(T a, T b) {
    return a < b ? a : b;
}

/**
 * @brief Get the maximum of two values
 * @tparam T Numeric type (must support comparison operators)
 * @param a First value
 * @param b Second value
 * @return T The larger value
 * 
 * @complexity{O(1)}
 * @since{1.0.0}
 * 
 * @example
 * ```cpp
 * i32 larger = math_utils::max(10, 20);   // 20
 * f64 max_val = math_utils::max(3.14, 2.71); // 3.14
 * ```
 */
template<typename T>
constexpr T max(T a, T b) {
    return a > b ? a : b;
}

/**
 * @brief Get the absolute value of a number
 * @tparam T Numeric type (must support unary minus)
 * @param value The value to get absolute value of
 * @return T The absolute value
 * 
 * @complexity{O(1)}
 * @since{1.0.0}
 * 
 * @usecase{Distance calculations}
 * Calculate distances without worrying about sign.
 * 
 * @example
 * ```cpp
 * i32 distance = math_utils::abs(-10);     // 10
 * f64 magnitude = math_utils::abs(-3.14);  // 3.14
 * ```
 */
template<typename T>
constexpr T abs(T value) {
    return value < 0 ? -value : value;
}

/** @} */

/**
 * @defgroup interpolation_functions Interpolation Functions
 * @brief Functions for interpolating between values
 * @{
 */

/**
 * @brief Linear interpolation between two values
 * @tparam T Numeric type
 * @param a Start value (when t = 0)
 * @param b End value (when t = 1)
 * @param t Interpolation factor (typically 0.0 to 1.0)
 * @return T Interpolated value
 * 
 * Calculates a + t * (b - a). When t = 0, returns a.
 * When t = 1, returns b. Values between give proportional results.
 * 
 * @complexity{O(1)}
 * @since{1.0.0}
 * 
 * @usecase{Animation}
 * Smoothly animate values between keyframes.
 * 
 * @usecase{Color blending}
 * Blend between two colors smoothly.
 * 
 * @usecase{Easing functions}
 * Create smooth transitions in user interfaces.
 * 
 * @example
 * ```cpp
 * // Basic interpolation
 * f64 half = math_utils::lerp(0.0, 100.0, 0.5);  // 50.0
 * f64 quarter = math_utils::lerp(0.0, 100.0, 0.25); // 25.0
 * 
 * // Color interpolation
 * u8 red_component = math_utils::lerp(255, 0, fade_factor);
 * 
 * // Animation position
 * f64 pos_x = math_utils::lerp(start_x, end_x, animation_time);
 * ```
 */
template<typename T>
constexpr T lerp(T a, T b, f64 t) {
    return static_cast<T>(a + t * (b - a));
}

/** @} */

// Power functions
template<typename T>
constexpr bool is_power_of_two(T value) {
    return value > 0 && (value & (value - 1)) == 0;
}

template<typename T>
T next_power_of_two(T value);

template<typename T>
T prev_power_of_two(T value);

// Rounding
f64 round_to_places(f64 value, i32 places);

// Random numbers
class Random {
private:
    std::mt19937 gen_;
    
public:
    Random();
    explicit Random(u32 seed);
    
    // Integer numbers
    i32 next_int();
    i32 next_int(i32 max);
    i32 next_int(i32 min, i32 max);
    
    // Floating point numbers
    f32 next_float();
    f32 next_float(f32 max);
    f32 next_float(f32 min, f32 max);
    
    f64 next_double();
    f64 next_double(f64 max);
    f64 next_double(f64 min, f64 max);
    
    // Boolean values
    bool next_bool();
    bool next_bool(f64 probability);
};

// Global random number generator
extern Random g_random;

// Fast random number functions
inline i32 random_int() { return g_random.next_int(); }
inline i32 random_int(i32 max) { return g_random.next_int(max); }
inline i32 random_int(i32 min, i32 max) { return g_random.next_int(min, max); }
inline f64 random_double() { return g_random.next_double(); }
inline f64 random_double(f64 max) { return g_random.next_double(max); }
inline f64 random_double(f64 min, f64 max) { return g_random.next_double(min, max); }
inline bool random_bool() { return g_random.next_bool(); }
inline bool random_bool(f64 probability) { return g_random.next_bool(probability); }

// Mathematical constants
constexpr f64 PI = 3.14159265358979323846;
constexpr f64 PI_2 = PI / 2.0;
constexpr f64 PI_4 = PI / 4.0;
constexpr f64 TAU = PI * 2.0;
constexpr f64 E = 2.71828182845904523536;
constexpr f64 SQRT2 = 1.41421356237309504880;
constexpr f64 SQRT3 = 1.73205080756887729352;
constexpr f64 PHI = 1.61803398874989484820; // Golden ratio

// Time utilities
void sleep_ms(u32 milliseconds);

} // namespace math_utils
} // namespace badcpplib 