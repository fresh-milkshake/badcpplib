#pragma once

#include "core.hpp"
#include <functional>
#include <algorithm>
#include <numeric>
#include <vector>
#include <optional>
#include <map>

namespace badcpplib {
namespace functional {

// Placeholder for functional programming

// Basic functional primitives

// Map - apply function to each element
template<typename Container, typename F>
auto map(const Container& container, F func) -> std::vector<decltype(func(*container.begin()))> {
    using ResultType = decltype(func(*container.begin()));
    std::vector<ResultType> result;
    result.reserve(container.size());
    
    for (const auto& item : container) {
        result.push_back(func(item));
    }
    
    return result;
}

// Filter - filter elements by predicate
template<typename Container, typename Predicate>
auto filter(const Container& container, Predicate pred) -> std::vector<typename Container::value_type> {
    std::vector<typename Container::value_type> result;
    
    for (const auto& item : container) {
        if (pred(item)) {
            result.push_back(item);
        }
    }
    
    return result;
}

// Reduce/Fold - fold container
template<typename Container, typename T, typename BinaryOp>
T reduce(const Container& container, T initial_value, BinaryOp op) {
    T result = initial_value;
    for (const auto& item : container) {
        result = op(result, item);
    }
    return result;
}

// Find - find first element satisfying predicate
template<typename Container, typename Predicate>
auto find_if(const Container& container, Predicate pred) -> std::optional<typename Container::value_type> {
    for (const auto& item : container) {
        if (pred(item)) {
            return item;
        }
    }
    return std::nullopt;
}

// All - check that all elements satisfy predicate
template<typename Container, typename Predicate>
bool all_of(const Container& container, Predicate pred) {
    for (const auto& item : container) {
        if (!pred(item)) {
            return false;
        }
    }
    return true;
}

// Any - check that at least one element satisfies predicate
template<typename Container, typename Predicate>
bool any_of(const Container& container, Predicate pred) {
    for (const auto& item : container) {
        if (pred(item)) {
            return true;
        }
    }
    return false;
}

// None - check that no element satisfies predicate
template<typename Container, typename Predicate>
bool none_of(const Container& container, Predicate pred) {
    return !any_of(container, pred);
}

// Count - count elements satisfying predicate
template<typename Container, typename Predicate>
usize count_if(const Container& container, Predicate pred) {
    usize count = 0;
    for (const auto& item : container) {
        if (pred(item)) {
            ++count;
        }
    }
    return count;
}

// Partition - partition container into two parts by predicate
template<typename Container, typename Predicate>
auto partition(const Container& container, Predicate pred) 
    -> std::pair<std::vector<typename Container::value_type>, 
                 std::vector<typename Container::value_type>> {
    
    std::vector<typename Container::value_type> true_part, false_part;
    
    for (const auto& item : container) {
        if (pred(item)) {
            true_part.push_back(item);
        } else {
            false_part.push_back(item);
        }
    }
    
    return {true_part, false_part};
}

// Take - take first N elements
template<typename Container>
auto take(const Container& container, usize n) -> std::vector<typename Container::value_type> {
    std::vector<typename Container::value_type> result;
    
    auto it = container.begin();
    for (usize i = 0; i < n && it != container.end(); ++i, ++it) {
        result.push_back(*it);
    }
    
    return result;
}

// Drop - skip first N elements
template<typename Container>
auto drop(const Container& container, usize n) -> std::vector<typename Container::value_type> {
    std::vector<typename Container::value_type> result;
    
    auto it = container.begin();
            // Skip first n elements
    for (usize i = 0; i < n && it != container.end(); ++i, ++it) {}
    
            // Add remaining
    for (; it != container.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

// Zip - combine two containers into pairs
template<typename Container1, typename Container2>
auto zip(const Container1& c1, const Container2& c2) 
    -> std::vector<std::pair<typename Container1::value_type, typename Container2::value_type>> {
    
    std::vector<std::pair<typename Container1::value_type, typename Container2::value_type>> result;
    
    auto it1 = c1.begin();
    auto it2 = c2.begin();
    
    while (it1 != c1.end() && it2 != c2.end()) {
        result.emplace_back(*it1, *it2);
        ++it1;
        ++it2;
    }
    
    return result;
}

// Enumerate - add indices to elements
template<typename Container>
auto enumerate(const Container& container) 
    -> std::vector<std::pair<usize, typename Container::value_type>> {
    
    std::vector<std::pair<usize, typename Container::value_type>> result;
    
    usize index = 0;
    for (const auto& item : container) {
        result.emplace_back(index++, item);
    }
    
    return result;
}

// Group By - group elements by key
template<typename Container, typename KeySelector>
auto group_by(const Container& container, KeySelector key_func) 
    -> std::vector<std::pair<decltype(key_func(*container.begin())), 
                            std::vector<typename Container::value_type>>> {
    
    using KeyType = decltype(key_func(*container.begin()));
    using ValueType = typename Container::value_type;
    
    std::vector<std::pair<KeyType, std::vector<ValueType>>> groups;
    
    for (const auto& item : container) {
        auto key = key_func(item);
        
        // Look for existing group
        auto group_it = std::find_if(groups.begin(), groups.end(),
                                   [&key](const auto& group) { return group.first == key; });
        
        if (group_it != groups.end()) {
            group_it->second.push_back(item);
        } else {
            groups.emplace_back(key, std::vector<ValueType>{item});
        }
    }
    
    return groups;
}

// Flatten - flatten nested containers
template<typename Container>
auto flatten(const Container& container) 
    -> std::vector<typename Container::value_type::value_type> {
    
    std::vector<typename Container::value_type::value_type> result;
    
    for (const auto& inner_container : container) {
        for (const auto& item : inner_container) {
            result.push_back(item);
        }
    }
    
    return result;
}

// Compose - function composition
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto&& x) -> decltype(f(g(x))) {
        return f(g(x));
    };
}

// Curry - function currying
template<typename F, typename Arg>
auto curry(F func, Arg arg) {
    return [func, arg](auto&&... args) -> decltype(func(arg, args...)) {
        return func(arg, args...);
    };
}

// Function to apply function n times
template<typename T, typename F>
T apply_n_times(T initial, usize n, F func) {
    T result = initial;
    for (usize i = 0; i < n; ++i) {
        result = func(result);
    }
    return result;
}

// Memoization - caching function results
template<typename F>
class Memoized {
private:
    F func_;
    mutable std::map<typename std::decay_t<F>::argument_type,
                     typename std::decay_t<F>::result_type> cache_;

public:
    explicit Memoized(F func) : func_(std::move(func)) {}
    
    template<typename Arg>
    auto operator()(Arg&& arg) const -> decltype(func_(arg)) {
        auto it = cache_.find(arg);
        if (it != cache_.end()) {
            return it->second;
        }
        
        auto result = func_(arg);
        cache_[arg] = result;
        return result;
    }
    
    void clear_cache() { cache_.clear(); }
    usize cache_size() const { return cache_.size(); }
};

template<typename F>
auto memoize(F func) -> Memoized<F> {
    return Memoized<F>(std::move(func));
}

// Pipeline - transformation chain
template<typename T>
class Pipeline {
private:
    T value_;
    
public:
    explicit Pipeline(T value) : value_(std::move(value)) {}
    
    template<typename F>
    auto pipe(F func) -> Pipeline<decltype(func(value_))> {
        return Pipeline<decltype(func(value_))>(func(value_));
    }
    
    const T& get() const { return value_; }
    T&& take() { return std::move(value_); }
};

template<typename T>
auto make_pipeline(T value) -> Pipeline<T> {
    return Pipeline<T>(std::move(value));
}

// Partial function application
template<typename F, typename... Args>
auto partial(F func, Args... args) {
    return [func, args...](auto&&... remaining_args) -> decltype(func(args..., remaining_args...)) {
        return func(args..., remaining_args...);
    };
}

// Identity function
template<typename T>
constexpr T identity(T&& value) {
    return std::forward<T>(value);
}

// Constant function
template<typename T>
auto constant(T value) {
    return [value](auto&&...) { return value; };
}

// Predicates
namespace predicates {
    
    template<typename T>
    auto equals(T value) {
        return [value](const T& other) { return other == value; };
    }
    
    template<typename T>
    auto greater_than(T value) {
        return [value](const T& other) { return other > value; };
    }
    
    template<typename T>
    auto less_than(T value) {
        return [value](const T& other) { return other < value; };
    }
    
    template<typename Predicate>
    auto negate(Predicate pred) {
        return [pred](auto&& value) { return !pred(value); };
    }
    
    template<typename P1, typename P2>
    auto logical_and(P1 p1, P2 p2) {
        return [p1, p2](auto&& value) { return p1(value) && p2(value); };
    }
    
    template<typename P1, typename P2>
    auto logical_or(P1 p1, P2 p2) {
        return [p1, p2](auto&& value) { return p1(value) || p2(value); };
    }
    
} // namespace predicates

// Utility functions for working with ranges
namespace ranges {
    
    // Generate sequence of numbers
    template<typename T>
    std::vector<T> range(T start, T end, T step = T{1}) {
        std::vector<T> result;
        
        if (step > T{0}) {
            for (T i = start; i < end; i += step) {
                result.push_back(i);
            }
        } else if (step < T{0}) {
            for (T i = start; i > end; i += step) {
                result.push_back(i);
            }
        }
        
        return result;
    }
    
    // Repeat value N times
    template<typename T>
    std::vector<T> repeat(T value, usize count) {
        return std::vector<T>(count, value);
    }
    
    // Cyclic repetition of sequence
    template<typename Container>
    std::vector<typename Container::value_type> cycle(const Container& container, usize total_count) {
        std::vector<typename Container::value_type> result;
        result.reserve(total_count);
        
        if (container.empty()) return result;
        
        usize container_size = container.size();
        for (usize i = 0; i < total_count; ++i) {
            auto it = container.begin();
            std::advance(it, i % container_size);
            result.push_back(*it);
        }
        
        return result;
    }
    
} // namespace ranges

} // namespace functional
} // namespace badcpplib 