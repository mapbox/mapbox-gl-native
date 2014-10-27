#include <iostream>
#include "gtest/gtest.h"

#include <mbgl/style/filter_comparison_private.hpp>
#include <mbgl/style/filter_expression_private.hpp>

#include <map>



class MockExtractor {
public:
    inline MockExtractor(const std::multimap<std::string, mbgl::Value> &values_) : values(values_) {}
    inline MockExtractor() {}

    inline std::vector<mbgl::Value> getValues(const std::string &key) const {
        std::vector<mbgl::Value> result;
        // Find all values with the requested key.
        const auto ret = values.equal_range(key);
        for (auto it = ret.first; it != ret.second; it++) {
            // Append the newly found value to the forward list.
            result.emplace_back(it->second);
        }
        return result;
    }

private:
    const std::multimap<std::string, mbgl::Value> values;
};


TEST(FilterComparison, EqualsSingleStringValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { std::string("bar") });
    // comparison is { "test": { "==": "bar" } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("barst") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(32.8) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}


TEST(FilterComparison, EqualsSingleDoubleValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { double(32.8) });
    // comparison is { "test": { "==": 32.8 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(32.8) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("32.8") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", double(32.8) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(32.8) }, { "test", std::string("booz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", double(32.8) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(32.9) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}

TEST(FilterComparison, EqualsSingleUintValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { uint64_t(42) });
    // comparison is { "test": { "==": 42 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("42") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", uint64_t(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(42) }, { "test", std::string("booz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", uint64_t(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(43) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}

TEST(FilterComparison, EqualsSingleIntValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { int64_t(-42) });
    // comparison is { "test": { "==": -42 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("-42") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", int64_t(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-42) }, { "test", std::string("booz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", int64_t(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-43) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(-43) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}

TEST(FilterComparison, EqualsSingleBoolValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { bool(true) });
    // comparison is { "test": { "==": true } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(1) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(1) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(1) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("1") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", bool(true) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(true) }, { "test", std::string("booz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", bool(true) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
}



TEST(FilterComparison, EqualsMultipleStringValues) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { std::string("bar"), std::string("baz") });
    // comparison is { "test": { "==": ["bar", "baz"]̇ } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("baz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("baz") }, { "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("baz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("baz") }, { "test", std::string("boo") }})));
}

TEST(FilterComparison, EqualsMultipleIdenticalStringValues) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::vector<Value> { std::string("bar"), std::string("bar") });
    // comparison is { "test": { "==": ["bar", "bar"]̇ } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("baz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("baz") }})));
}



TEST(FilterComparison, NotEqualsSingleStringValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { std::string("bar") });
    // comparison is { "test": { "!=": "bar" } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor()));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("barst") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(32.8) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}


TEST(FilterComparison, NotEqualsSingleDoubleValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { double(32.8) });
    // comparison is { "test": { "!=": 32.8 } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(32.8) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("32.8") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", double(32.8) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(32.8) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", double(32.8) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor()));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(32.9) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}

TEST(FilterComparison, NotEqualsSingleUintValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { uint64_t(42) });
    // comparison is { "test": { "!=": 42 } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("42") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", uint64_t(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(42) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", uint64_t(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor()));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(43) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}


TEST(FilterComparison, NotEqualsSingleIntValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { int64_t(-42) });
    // comparison is { "test": { "!=": -42 } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("-42") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", int64_t(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-42) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", int64_t(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor()));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-43) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(-43) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
}


TEST(FilterComparison, NotEqualsSingleBoolValue) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { bool(true) });
    // comparison is { "test": { "!=": true } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(1) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(1) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(1) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("1") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", bool(true) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", bool(true) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor()));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
}



TEST(FilterComparison, NotEqualsMultipleStringValues) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { std::string("bar"), std::string("baz") });
    // comparison is { "test": { "!=": ["bar", "baz"]̇ } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("baz") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("baz") }, { "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("baz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("baz") }, { "test", std::string("boo") }})));
}



TEST(FilterComparison, NotEqualsMultipleIdenticalStringValues) {
    using namespace mbgl;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::NotEqual, std::vector<Value> { std::string("bar"), std::string("bar") });
    // comparison is { "test": { "!=": ["bar", "bar"]̇ } }

    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("baz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("baz") }})));
}
