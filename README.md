# BadCppLib

A C++17 utility library providing common functionality for C++ projects.

## Overview

BadCppLib is a collection of utilities organized into modules. The library uses header-only and source implementations depending on the module.

## Requirements

### Minimum Requirements
- C++17 compatible compiler (tested with GCC 7+, Clang 6+, MSVC 2017+)
- Python 3.6+ (for build scripts)

### Optional
- CMake 3.10+ (note: CMake builds have linking issues)
- Doxygen 1.8+ (for documentation generation)
- Make (for Unix-like systems)

## Installation

Clone the repository:
```bash
git clone https://github.com/fresh-milkshake/badcpplib
cd badcpplib
```

Include in your project:
```cpp
#include "badcpplib.hpp"
```

Compile with:
```bash
g++ -std=c++17 -I./include your_file.cpp src/badcpplib.cpp -o your_program
```

## Available Modules

### Core Types
Basic type aliases:
- Integer types: `i8`, `i16`, `i32`, `i64`, `i128` (GCC/Clang only)
- Unsigned types: `u8`, `u16`, `u32`, `u64`, `u128` (GCC/Clang only)
- Floating-point types: `f32`, `f64`, `f128` (long double)
- Size types: `usize` (size_t), `isize` (ptrdiff_t)
- Container aliases: `string`, `vector<T>`, `map<K,V>` (unordered_map)

### Result Type
Error handling without exceptions:
```cpp
Result<string> read_file(const string& path);
```
Methods: `is_ok()`, `is_err()`, `unwrap()`, `unwrap_or()`, `error()`

### String Utilities
- `split(str, delimiter)` - Split string by delimiter
- `join(vector, delimiter)` - Join strings with delimiter
- `trim(str)` - Remove leading/trailing whitespace
- `to_upper(str)` / `to_lower(str)` - Case conversion
- `contains(str, substr)` - Check substring presence
- `starts_with(str, prefix)` / `ends_with(str, suffix)` - Prefix/suffix check

### Mathematical Utilities
- `clamp(value, min, max)` - Constrain value to range
- `lerp(a, b, t)` - Linear interpolation
- `is_power_of_two(n)` - Power of two check
- `next_power_of_two(n)` - Next power of two
- `random_int(min, max)` / `random_double(min, max)` - Random number generation

### Time Utilities
- `Timer` class for elapsed time measurement
- `measure_time(func)` - Measure function execution time
- `sleep_ms(milliseconds)` - Thread sleep

### Containers
- `StaticArray<T, N>` - Fixed-size array
- `CircularBuffer<T>` - Ring buffer

### Functional Programming
- `filter(container, predicate)` - Filter elements
- `map(container, func)` - Transform elements
- `reduce(container, initial, func)` - Fold/accumulate
- `any(container, predicate)` / `all(container, predicate)` - Predicates

### Memory Management
- `make_unique<T>(args...)` / `make_shared<T>(args...)` - Smart pointer creation
- `ScopeGuard` - RAII cleanup utility
- `SCOPE_EXIT(code)` - Macro for scope exit actions

### Debug Utilities
- `debug::print(args...)` - Variadic print function
- `debug::print_container(container)` - Container printing
- `debug::assert_true(condition, message)` - Runtime assertion (throws exception)

### File Utilities
- `read_file(path)` - Read file content (returns Result<string>)
- `write_file(path, content)` - Write file (returns Result<bool>)
- `file_exists(path)` - Check file existence

## Project Structure

```
badcpplib/
├── include/
│   ├── badcpplib.hpp           # Single-header include
│   └── badcpplib/              # Modular headers
├── src/
│   ├── badcpplib.cpp           # Main implementation
│   └── modules/                # Module implementations
├── examples/
│   └── example.cpp             # Usage example
├── tests/
│   ├── basic_test.cpp          # Basic tests
│   └── modules/                # Module tests
├── scripts/
│   ├── build.py                # Build script
│   ├── modular_build.py        # Modular build script
│   └── generate_docs.py        # Documentation generator
└── CMakeLists.txt              # CMake configuration
```

## Build Scripts

### build.py
Cross-platform build script with compiler auto-detection.

```bash
# Direct compilation (recommended)
python scripts/build.py --direct --run

# With tests
python scripts/build.py --direct --test

# Show options
python scripts/build.py --help
```

### modular_build.py
Build specific modules with dependency resolution.

```bash
# List modules
python scripts/modular_build.py --list-modules

# Build specific modules
python scripts/modular_build.py --modules string_utils math_utils

# Build and test all
python scripts/modular_build.py --modules all --build-tests --run-tests
```

### generate_docs.py
Generate documentation using Doxygen.

```bash
# Generate HTML docs
python scripts/generate_docs.py --format html

# Serve documentation
python scripts/generate_docs.py --serve 8080
```

## Usage Examples

### Basic Usage
```cpp
#include "badcpplib.hpp"
using namespace badcpplib;

int main() {
    // Types
    i32 number = 42;
    string text = "Hello";
    
    // String operations
    auto words = string_utils::split("a,b,c", ',');
    string joined = string_utils::join(words, " ");
    
    // Error handling
    auto result = file_utils::read_file("config.txt");
    if (result.is_ok()) {
        debug::print("Content:", result.unwrap());
    }
    
    return 0;
}
```

### Container Example
```cpp
// StaticArray - fixed size, no bounds checking on operator[]
containers::StaticArray<i32, 10> arr;
arr.push(1);
arr.push(2);
arr.push(3);
// Access: arr[0], arr[1], arr[2]

// CircularBuffer - overwrites oldest elements when full
containers::CircularBuffer<string> buffer(3);
buffer.push("first");
buffer.push("second");
buffer.push("third");
buffer.push("fourth"); // Overwrites "first"
```

### Functional Example
```cpp
vector<i32> nums = {1, 2, 3, 4, 5};

auto evens = functional::filter(nums, [](i32 n) { 
    return n % 2 == 0; 
});

i32 sum = functional::reduce(nums, 0, [](i32 a, i32 b) { 
    return a + b; 
});
```

## Testing

Run tests using:

```bash
# Using build script
python scripts/build.py --direct --test

# Manual compilation
g++ -std=c++17 -I./include tests/basic_test.cpp src/badcpplib.cpp -o test_runner
./test_runner
```

## Known Issues and Limitations

### Technical Limitations
- Template-heavy code increases compilation time
- Static library compilation has symbol duplication (use direct compilation recommended)
- No bounds checking in `StaticArray::operator[]`
- `CircularBuffer` silently overwrites old data when full
- `Result::unwrap()` throws exceptions (not noexcept)

### Design Limitations
- Some modules have interdependencies
- Limited to C++17 features
- No header-only option for all modules
- No namespace versioning
- No thread safety guarantees in containers

### Missing Features
- No package manager integration
- No CMake install targets
- No pkg-config support
- No single-header distribution

And overall - it's a bad cpp library.

## License

This project is distributed under the MIT license. Details can be found in the [LICENSE](LICENSE.txt) file.