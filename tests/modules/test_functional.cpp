#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_FUNCTIONAL
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("Functional Module Tests");
    
    TEST_CASE("Map function")
        std::vector<i32> numbers = {1, 2, 3, 4, 5};
        
        // Multiply by 2
        auto doubled = functional::map(numbers, [](i32 x) { return x * 2; });
        std::vector<i32> expected_doubled = {2, 4, 6, 8, 10};
        ASSERT_EQ(doubled.size(), expected_doubled.size());
        for (usize i = 0; i < doubled.size(); ++i) {
            ASSERT_EQ(doubled[i], expected_doubled[i]);
        }
        
        // Convert to strings
        auto strings = functional::map(numbers, [](i32 x) { return std::to_string(x); });
        ASSERT_EQ(strings.size(), 5u);
        ASSERT_EQ(strings[0], "1");
        ASSERT_EQ(strings[4], "5");
        
        // Empty container
        std::vector<i32> empty;
        auto empty_result = functional::map(empty, [](i32 x) { return x * 2; });
        ASSERT_TRUE(empty_result.empty());
    TEST_END
    
    TEST_CASE("Filter function")
        std::vector<i32> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        // Even numbers
        auto evens = functional::filter(numbers, [](i32 x) { return x % 2 == 0; });
        std::vector<i32> expected_evens = {2, 4, 6, 8, 10};
        ASSERT_EQ(evens.size(), expected_evens.size());
        for (usize i = 0; i < evens.size(); ++i) {
            ASSERT_EQ(evens[i], expected_evens[i]);
        }
        
        // Numbers greater than 5
        auto greater_than_5 = functional::filter(numbers, [](i32 x) { return x > 5; });
        std::vector<i32> expected_gt5 = {6, 7, 8, 9, 10};
        ASSERT_EQ(greater_than_5.size(), expected_gt5.size());
        for (usize i = 0; i < greater_than_5.size(); ++i) {
            ASSERT_EQ(greater_than_5[i], expected_gt5[i]);
        }
        
        // Filter that passes nothing
        auto none = functional::filter(numbers, [](i32 x) { return x > 100; });
        ASSERT_TRUE(none.empty());
    TEST_END
    
    TEST_CASE("Reduce function")
        std::vector<i32> numbers = {1, 2, 3, 4, 5};
        
        // Sum
        auto sum = functional::reduce(numbers, 0, [](i32 acc, i32 x) { return acc + x; });
        ASSERT_EQ(sum, 15);
        
        // Product
        auto product = functional::reduce(numbers, 1, [](i32 acc, i32 x) { return acc * x; });
        ASSERT_EQ(product, 120);
        
        // Maximum
        auto max_val = functional::reduce(numbers, 0, [](i32 acc, i32 x) { return std::max(acc, x); });
        ASSERT_EQ(max_val, 5);
        
        // Empty container returns initial value
        std::vector<i32> empty;
        auto empty_sum = functional::reduce(empty, 42, [](i32 acc, i32 x) { return acc + x; });
        ASSERT_EQ(empty_sum, 42);
    TEST_END
    
    TEST_CASE("Predicates (all_of, any_of, none_of)")
        std::vector<i32> numbers = {2, 4, 6, 8, 10};
        std::vector<i32> mixed = {1, 2, 3, 4, 5};
        std::vector<i32> odds = {1, 3, 5, 7, 9};
        
        // all_of
        ASSERT_TRUE(functional::all_of(numbers, [](i32 x) { return x % 2 == 0; }));
        ASSERT_FALSE(functional::all_of(mixed, [](i32 x) { return x % 2 == 0; }));
        
        // any_of
        ASSERT_TRUE(functional::any_of(mixed, [](i32 x) { return x % 2 == 0; }));
        ASSERT_FALSE(functional::any_of(odds, [](i32 x) { return x % 2 == 0; }));
        
        // none_of
        ASSERT_TRUE(functional::none_of(odds, [](i32 x) { return x % 2 == 0; }));
        ASSERT_FALSE(functional::none_of(mixed, [](i32 x) { return x % 2 == 0; }));
    TEST_END
    
    TEST_CASE("Find and count functions")
        std::vector<i32> numbers = {1, 2, 3, 4, 5, 4, 3, 2, 1};
        
        // find_if
        auto found = functional::find_if(numbers, [](i32 x) { return x > 3; });
        ASSERT_TRUE(found.has_value());
        ASSERT_EQ(found.value(), 4);
        
        auto not_found = functional::find_if(numbers, [](i32 x) { return x > 10; });
        ASSERT_FALSE(not_found.has_value());
        
        // count_if
        auto count_evens = functional::count_if(numbers, [](i32 x) { return x % 2 == 0; });
        ASSERT_EQ(count_evens, 4u); // 2, 4, 4, 2
        
        auto count_gt3 = functional::count_if(numbers, [](i32 x) { return x > 3; });
        ASSERT_EQ(count_gt3, 3u); // 4, 5, 4
    TEST_END
    
    TEST_CASE("Partition function")
        std::vector<i32> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        auto [evens, odds] = functional::partition(numbers, [](i32 x) { return x % 2 == 0; });
        
        std::vector<i32> expected_evens = {2, 4, 6, 8, 10};
        std::vector<i32> expected_odds = {1, 3, 5, 7, 9};
        
        ASSERT_EQ(evens.size(), expected_evens.size());
        ASSERT_EQ(odds.size(), expected_odds.size());
        for (usize i = 0; i < evens.size(); ++i) {
            ASSERT_EQ(evens[i], expected_evens[i]);
        }
        for (usize i = 0; i < odds.size(); ++i) {
            ASSERT_EQ(odds[i], expected_odds[i]);
        }
    TEST_END
    
    TEST_CASE("Take and drop functions")
        std::vector<i32> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        // take
        auto first_5 = functional::take(numbers, 5);
        std::vector<i32> expected_first5 = {1, 2, 3, 4, 5};
        ASSERT_EQ(first_5.size(), expected_first5.size());
        for (usize i = 0; i < first_5.size(); ++i) {
            ASSERT_EQ(first_5[i], expected_first5[i]);
        }
        
        auto take_more = functional::take(numbers, 20); // More than available
        ASSERT_EQ(take_more.size(), numbers.size());
        for (usize i = 0; i < take_more.size(); ++i) {
            ASSERT_EQ(take_more[i], numbers[i]);
        }
        
        auto take_zero = functional::take(numbers, 0);
        ASSERT_TRUE(take_zero.empty());
        
        // drop
        auto skip_3 = functional::drop(numbers, 3);
        std::vector<i32> expected_skip3 = {4, 5, 6, 7, 8, 9, 10};
        ASSERT_EQ(skip_3.size(), expected_skip3.size());
        for (usize i = 0; i < skip_3.size(); ++i) {
            ASSERT_EQ(skip_3[i], expected_skip3[i]);
        }
        
        auto drop_all = functional::drop(numbers, 15);
        ASSERT_TRUE(drop_all.empty());
    TEST_END
    
    TEST_CASE("Zip and enumerate functions")
        std::vector<i32> numbers = {1, 2, 3};
        std::vector<string> words = {"one", "two", "three"};
        
        // zip
        auto zipped = functional::zip(numbers, words);
        ASSERT_EQ(zipped.size(), 3u);
        ASSERT_EQ(zipped[0].first, 1);
        ASSERT_EQ(zipped[0].second, "one");
        ASSERT_EQ(zipped[2].first, 3);
        ASSERT_EQ(zipped[2].second, "three");
        
        // enumerate
        auto enumerated = functional::enumerate(words);
        ASSERT_EQ(enumerated.size(), 3u);
        ASSERT_EQ(enumerated[0].first, 0u);
        ASSERT_EQ(enumerated[0].second, "one");
        ASSERT_EQ(enumerated[2].first, 2u);
        ASSERT_EQ(enumerated[2].second, "three");
    TEST_END
    
    TEST_CASE("Group by function")
        std::vector<string> words = {"cat", "dog", "bird", "cow", "ant", "bee"};
        
        // Group by length
        auto groups = functional::group_by(words, [](const string& s) { return s.length(); });
        
        ASSERT_EQ(groups.size(), 2u); // Groups: 3, 4 characters
        
        // Check that there is a group with 3-character words
        auto group_3 = std::find_if(groups.begin(), groups.end(),
                                   [](const auto& group) { return group.first == 3; });
        ASSERT_TRUE(group_3 != groups.end());
        ASSERT_EQ(group_3->second.size(), 5u); // cat, dog, cow, ant, bee
    TEST_END
    
    TEST_CASE("Flatten function")
        std::vector<std::vector<i32>> nested = {{1, 2}, {3, 4, 5}, {6}};
        
        auto flattened = functional::flatten(nested);
        std::vector<i32> expected = {1, 2, 3, 4, 5, 6};
        ASSERT_EQ(flattened.size(), expected.size());
        for (usize i = 0; i < flattened.size(); ++i) {
            ASSERT_EQ(flattened[i], expected[i]);
        }
        
        // Empty nested containers
        std::vector<std::vector<i32>> with_empty = {{1}, {}, {2, 3}};
        auto flattened_empty = functional::flatten(with_empty);
        std::vector<i32> expected_empty = {1, 2, 3};
        ASSERT_EQ(flattened_empty.size(), expected_empty.size());
        for (usize i = 0; i < flattened_empty.size(); ++i) {
            ASSERT_EQ(flattened_empty[i], expected_empty[i]);
        }
    TEST_END
    
    TEST_CASE("Function composition")
        auto add_one = [](i32 x) { return x + 1; };
        auto multiply_by_2 = [](i32 x) { return x * 2; };
        
        // Composition: first +1, then *2
        auto composed = functional::compose(multiply_by_2, add_one);
        ASSERT_EQ(composed(5), 12); // (5 + 1) * 2 = 12
        
        // Reverse composition: first *2, then +1
        auto reversed = functional::compose(add_one, multiply_by_2);
        ASSERT_EQ(reversed(5), 11); // (5 * 2) + 1 = 11
    TEST_END
    
    TEST_CASE("Curry and partial application")
        auto add = [](i32 a, i32 b) { return a + b; };
        auto multiply_3 = [](i32 a, i32 b, i32 c) { return a * b * c; };
        
        // Curry - fix first argument
        auto add_5 = functional::curry(add, 5);
        ASSERT_EQ(add_5(3), 8);
        
        // Partial application
        auto multiply_by_2_and_3 = functional::partial(multiply_3, 2, 3);
        ASSERT_EQ(multiply_by_2_and_3(4), 24); // 2 * 3 * 4 = 24
    TEST_END
    
    TEST_CASE("Utility functions")
        // apply_n_times
        auto increment = [](i32 x) { return x + 1; };
        auto result = functional::apply_n_times(10, 5, increment);
        ASSERT_EQ(result, 15);
        
        // identity
        ASSERT_EQ(functional::identity(42), 42);
        ASSERT_EQ(functional::identity("hello"), "hello");
        
        // constant
        auto const_5 = functional::constant(5);
        ASSERT_EQ(const_5(), 5);
        ASSERT_EQ(const_5(1, 2, 3), 5); // Ignores arguments
    TEST_END
    
    TEST_CASE("Memoization")
        // Fibonacci function (expensive to compute)
        std::function<i32(i32)> slow_fib = [](i32 n) -> i32 {
            if (n <= 1) return n;
            return n * 2; // Simplified version for testing
        };
        
        auto memoized_fib = functional::memoize(slow_fib);
        
        // First call
        auto result1 = memoized_fib(10);
        ASSERT_EQ(result1, 20);
        
        // Second call (should use cache)
        auto result2 = memoized_fib(10);
        ASSERT_EQ(result2, 20);
        
        // Check cache size
        ASSERT_EQ(memoized_fib.cache_size(), 1u);
        
        // Different value
        auto result3 = memoized_fib(5);
        ASSERT_EQ(result3, 10);
        ASSERT_EQ(memoized_fib.cache_size(), 2u);
        
        // Clear cache
        memoized_fib.clear_cache();
        ASSERT_EQ(memoized_fib.cache_size(), 0u);
    TEST_END
    
    TEST_CASE("Pipeline operations")
        auto pipeline = functional::make_pipeline(5)
            .pipe([](i32 x) { return x * 2; })    // 10
            .pipe([](i32 x) { return x + 3; })    // 13
            .pipe([](i32 x) { return x * x; });   // 169
        
        ASSERT_EQ(pipeline.get(), 169);
        
        // String operation chain (commented out due to dependency on string_utils)
        // auto str_pipeline = functional::make_pipeline(string("hello"))
        //     .pipe([](const string& s) { return s + " world"; })
        //     .pipe([](const string& s) { return string_utils::to_uppercase(s); });
        // ASSERT_EQ(str_pipeline.get(), "HELLO WORLD");
    TEST_END
    
    TEST_CASE("Predicate combinators")
        auto is_even = functional::predicates::equals(0);
        auto gt_5 = functional::predicates::greater_than(5);
        auto lt_10 = functional::predicates::less_than(10);
        
        // Test basic predicates
        ASSERT_TRUE(gt_5(8));
        ASSERT_FALSE(gt_5(3));
        ASSERT_TRUE(lt_10(7));
        ASSERT_FALSE(lt_10(15));
        
        // Logical operations
        auto range_5_to_10 = functional::predicates::logical_and(gt_5, lt_10);
        ASSERT_TRUE(range_5_to_10(7));
        ASSERT_FALSE(range_5_to_10(3));
        ASSERT_FALSE(range_5_to_10(12));
        
        // Negation
        auto not_gt_5 = functional::predicates::negate(gt_5);
        ASSERT_FALSE(not_gt_5(8));
        ASSERT_TRUE(not_gt_5(3));
    TEST_END
    
    TEST_CASE("Range utilities")
        // range
        auto range_1_to_5 = functional::ranges::range(1, 6); // [1, 6)
        std::vector<i32> expected_range = {1, 2, 3, 4, 5};
        ASSERT_EQ(range_1_to_5.size(), expected_range.size());
        for (usize i = 0; i < range_1_to_5.size(); ++i) {
            ASSERT_EQ(range_1_to_5[i], expected_range[i]);
        }
        
        auto range_with_step = functional::ranges::range(0, 10, 2);
        std::vector<i32> expected_step = {0, 2, 4, 6, 8};
        ASSERT_EQ(range_with_step.size(), expected_step.size());
        for (usize i = 0; i < range_with_step.size(); ++i) {
            ASSERT_EQ(range_with_step[i], expected_step[i]);
        }
        
        // repeat
        auto repeated = functional::ranges::repeat(42, 3);
        ASSERT_EQ(repeated.size(), 3u);
        for (const auto& val : repeated) {
            ASSERT_EQ(val, 42);
        }
        
        // cycle
        std::vector<i32> base = {1, 2, 3};
        auto cycled = functional::ranges::cycle(base, 7);
        std::vector<i32> expected_cycle = {1, 2, 3, 1, 2, 3, 1};
        ASSERT_EQ(cycled.size(), expected_cycle.size());
        for (usize i = 0; i < cycled.size(); ++i) {
            ASSERT_EQ(cycled[i], expected_cycle[i]);
        }
    TEST_END
    
    RUN_ALL_TESTS();
} 