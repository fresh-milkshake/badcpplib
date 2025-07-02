#include "badcpplib/math_utils.hpp"
#include <cmath>

namespace badcpplib {
namespace math_utils {

template<typename T>
T next_power_of_two(T value) {
    if (value <= 1) return 1;
    
    --value;
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

template<typename T>
T prev_power_of_two(T value) {
    if (value <= 1) return 1;
    if (is_power_of_two(value)) return value;
    return next_power_of_two(value) >> 1;
}

// Explicit instantiations
template i32 next_power_of_two<i32>(i32 value);
template u32 next_power_of_two<u32>(u32 value);
template u64 next_power_of_two<u64>(u64 value);
template i32 prev_power_of_two<i32>(i32 value);
template u32 prev_power_of_two<u32>(u32 value);
template u64 prev_power_of_two<u64>(u64 value);

f64 round_to_places(f64 value, i32 places) {
    f64 multiplier = std::pow(10.0, places);
    return std::round(value * multiplier) / multiplier;
}

// Random class implementation
Random::Random() : gen_(std::random_device{}()) {}

Random::Random(u32 seed) : gen_(seed) {}

i32 Random::next_int() {
    std::uniform_int_distribution<i32> dist;
    return dist(gen_);
}

i32 Random::next_int(i32 max) {
    std::uniform_int_distribution<i32> dist(0, max - 1);
    return dist(gen_);
}

i32 Random::next_int(i32 min, i32 max) {
    std::uniform_int_distribution<i32> dist(min, max);
    return dist(gen_);
}

f64 Random::next_double() {
    std::uniform_real_distribution<f64> dist(0.0, 1.0);
    return dist(gen_);
}

f64 Random::next_double(f64 max) {
    std::uniform_real_distribution<f64> dist(0.0, max);
    return dist(gen_);
}

f64 Random::next_double(f64 min, f64 max) {
    std::uniform_real_distribution<f64> dist(min, max);
    return dist(gen_);
}

f32 Random::next_float() {
    std::uniform_real_distribution<f32> dist(0.0f, 1.0f);
    return dist(gen_);
}

f32 Random::next_float(f32 max) {
    std::uniform_real_distribution<f32> dist(0.0f, max);
    return dist(gen_);
}

f32 Random::next_float(f32 min, f32 max) {
    std::uniform_real_distribution<f32> dist(min, max);
    return dist(gen_);
}

bool Random::next_bool() {
    std::uniform_int_distribution<i32> dist(0, 1);
    return dist(gen_) == 1;
}

bool Random::next_bool(f64 probability) {
    return next_double() < probability;
}

// Global generator
Random g_random;

void sleep_ms(u32 milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

} // namespace math_utils
} // namespace badcpplib 