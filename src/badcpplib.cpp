// badcpplib.cpp : Defines functions for the static library.
//

#include "pch.hpp"
#include "framework.hpp"
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
        
        ErrorWrapper(E&& e) : error(std::forward<E>(e)) {}
        ErrorWrapper(const E& e) : error(e) {}
    };
    
    template<typename T, typename E = string>
    class Result {
    private:
        variant<T, ErrorWrapper<E>> data_;

    public:
        // Static methods for creation
        static Result ok(T&& value) {
            return Result(std::forward<T>(value));
        }
        
        static Result ok(const T& value) {
            return Result(value);
        }
        
        static Result err(E&& error) {
            return Result(ErrorWrapper<E>(std::forward<E>(error)));
        }
        
        static Result err(const E& error) {
            return Result(ErrorWrapper<E>(error));
        }

    private:
        Result(T&& value) : data_(std::forward<T>(value)) {}
        Result(const T& value) : data_(value) {}
        Result(ErrorWrapper<E>&& wrapper) : data_(std::move(wrapper)) {}

    public:

		        bool is_ok() const { 
            return std::holds_alternative<T>(data_); 
        }
        
        bool is_err() const { 
            return std::holds_alternative<ErrorWrapper<E>>(data_); 
        }

        T& unwrap() {
            if (is_err()) throw std::runtime_error("Called unwrap on an Err value");
            return std::get<T>(data_);
        }

        const T& unwrap() const {
            if (is_err()) throw std::runtime_error("Called unwrap on an Err value");
            return std::get<T>(data_);
        }

        T unwrap_or(T&& default_value) {
            return is_ok() ? std::get<T>(data_) : default_value;
        }

        E& error() {
            if (is_ok()) throw std::runtime_error("Called error on an Ok value");
            return std::get<ErrorWrapper<E>>(data_).error;
        }

        const E& error() const {
            if (is_ok()) throw std::runtime_error("Called error on an Ok value");
            return std::get<ErrorWrapper<E>>(data_).error;
        }

        template<typename F>
        auto map(F&& func) -> Result<decltype(func(std::get<T>(data_))), E> {
            if (is_ok()) {
                return Result<decltype(func(std::get<T>(data_))), E>::ok(func(std::get<T>(data_)));
            }
            return Result<decltype(func(std::get<T>(data_))), E>::err(error());
        }
	};

	// ===== STRING UTILITIES =====
	namespace string_utils {
		vector<string> split(const string& str, char delimiter) {
			vector<string> result;
			std::stringstream ss(str);
			string token;
			while (std::getline(ss, token, delimiter)) {
				result.push_back(token);
			}
			return result;
		}

		string join(const vector<string>& strings, const string& delimiter) {
			if (strings.empty()) return "";
			string result = strings[0];
			for (usize i = 1; i < strings.size(); ++i) {
				result += delimiter + strings[i];
			}
			return result;
		}

		string trim(const string& str) {
			auto start = str.find_first_not_of(" \t\n\r");
			if (start == string::npos) return "";
			auto end = str.find_last_not_of(" \t\n\r");
			return str.substr(start, end - start + 1);
		}

		string to_lower(const string& str) {
			string result = str;
			std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			return result;
		}

		string to_upper(const string& str) {
			string result = str;
			std::transform(result.begin(), result.end(), result.begin(), ::toupper);
			return result;
		}

		bool starts_with(const string& str, const string& prefix) {
			return str.size() >= prefix.size() && 
				   str.compare(0, prefix.size(), prefix) == 0;
		}

		bool ends_with(const string& str, const string& suffix) {
			return str.size() >= suffix.size() && 
				   str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
		}

		bool contains(const string& str, const string& substring) {
			return str.find(substring) != string::npos;
		}
	}

	// ===== FILE OPERATIONS =====
	namespace file_utils {
		        Result<string> read_file(const string& filepath) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return Result<string>::err("Failed to open file: " + filepath);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            return Result<string>::ok(buffer.str());
        }

        Result<bool> write_file(const string& filepath, const string& content) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return Result<bool>::err("Failed to create file: " + filepath);
            }

            file << content;
            return Result<bool>::ok(true);
        }

		bool file_exists(const string& filepath) {
			std::ifstream file(filepath);
			return file.good();
		}
	}

	// ===== MATHEMATICAL UTILITIES =====
	namespace math_utils {
		template<typename T>
		T clamp(T value, T min_val, T max_val) {
			return std::max(min_val, std::min(value, max_val));
		}

		template<typename T>
		T lerp(T a, T b, f64 t) {
			return a + static_cast<T>((b - a) * t);
		}

		template<typename T>
		bool is_power_of_two(T value) {
			return value > 0 && (value & (value - 1)) == 0;
		}

		template<typename T>
		T next_power_of_two(T value) {
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

		f64 random_double(f64 min = 0.0, f64 max = 1.0) {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_real_distribution<f64> dist(min, max);
			return dist(gen);
		}

		i32 random_int(i32 min, i32 max) {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<i32> dist(min, max);
			return dist(gen);
		}
	}

	    // ===== TIME MEASUREMENT =====
    namespace time_utils {

		

		
	}

	// ===== CONTAINERS AND ALGORITHMS =====
	namespace containers {
		template<typename T, usize N>
		class StaticArray {
		private:
			T data_[N];
			usize size_;

		public:
			StaticArray() : size_(0) {}

			void push(const T& item) {
				if (size_ < N) {
					data_[size_++] = item;
				}
			}

			void pop() {
				if (size_ > 0) {
					size_--;
				}
			}

			T& operator[](usize index) {
				return data_[index];
			}

			const T& operator[](usize index) const {
				return data_[index];
			}

			usize size() const { return size_; }
			usize capacity() const { return N; }
			bool empty() const { return size_ == 0; }
			bool full() const { return size_ == N; }

			T* begin() { return data_; }
			T* end() { return data_ + size_; }
			const T* begin() const { return data_; }
			const T* end() const { return data_ + size_; }
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
			CircularBuffer(usize capacity) 
				: buffer_(capacity), head_(0), tail_(0), size_(0), capacity_(capacity) {}

			void push(const T& item) {
				buffer_[tail_] = item;
				tail_ = (tail_ + 1) % capacity_;
				if (size_ < capacity_) {
					size_++;
				} else {
					head_ = (head_ + 1) % capacity_;
				}
			}

			optional<T> pop() {
				if (size_ == 0) return std::nullopt;
				T item = buffer_[head_];
				head_ = (head_ + 1) % capacity_;
				size_--;
				return item;
			}

			usize size() const { return size_; }
			bool empty() const { return size_ == 0; }
			bool full() const { return size_ == capacity_; }
		};
	}

	// ===== FUNCTIONAL PROGRAMMING =====
	namespace functional {
		template<typename Container, typename Predicate>
		auto filter(const Container& container, Predicate pred) {
			Container result;
			std::copy_if(container.begin(), container.end(), 
						std::back_inserter(result), pred);
			return result;
		}

		template<typename Container, typename Func>
		auto map(const Container& container, Func func) {
			using ValueType = decltype(func(*container.begin()));
			std::vector<ValueType> result;
			std::transform(container.begin(), container.end(), 
						  std::back_inserter(result), func);
			return result;
		}

		template<typename Container, typename T, typename Func>
		T reduce(const Container& container, T initial, Func func) {
			return std::accumulate(container.begin(), container.end(), initial, func);
		}

		template<typename Container, typename Predicate>
		bool any(const Container& container, Predicate pred) {
			return std::any_of(container.begin(), container.end(), pred);
		}

		template<typename Container, typename Predicate>
		bool all(const Container& container, Predicate pred) {
			return std::all_of(container.begin(), container.end(), pred);
		}
	}

	// ===== DEBUG UTILITIES =====
	namespace debug {
		template<typename T>
		void print(const T& value) {
			std::cout << value << std::endl;
		}

		template<typename T, typename... Args>
		void print(const T& first, const Args&... args) {
			std::cout << first << " ";
			print(args...);
		}

		template<typename Container>
		void print_container(const Container& container) {
			std::cout << "[";
			bool first = true;
			for (const auto& item : container) {
				if (!first) std::cout << ", ";
				std::cout << item;
				first = false;
			}
			std::cout << "]" << std::endl;
		}

		void assert_true(bool condition, const string& message = "Assertion failed") {
			if (!condition) {
				throw std::runtime_error(message);
			}
		}
	}

	// ===== MEMORY AND RESOURCES =====
	namespace memory {
		template<typename T, typename... Args>
		unique_ptr<T> make_unique(Args&&... args) {
			return std::make_unique<T>(std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		shared_ptr<T> make_shared(Args&&... args) {
			return std::make_shared<T>(std::forward<Args>(args)...);
		}

		

		#define SCOPE_EXIT(code) \
			auto scope_guard_##__LINE__ = memory::ScopeGuard([&]() { code; })
	}
}