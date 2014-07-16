#include <iostream>
#include "gtest/gtest.h"

#include <llmr/style/filter_expression.hpp>
#include <llmr/style/filter_expression_private.hpp>

#include <map>



class MockExtractor {
public:
    MockExtractor(const std::multimap<std::string, llmr::Value> &values) : values(values) {}
    MockExtractor() {}

    inline std::forward_list<llmr::Value> getValues(const std::string &key) const {
        std::forward_list<llmr::Value> result;
        auto rit = result.before_begin();
        // Find all values with the requested key.
        const auto ret = values.equal_range(key);
        for (auto it = ret.first; it != ret.second; it++) {
            // Append the newly found value to the forward list.
            rit = result.emplace_after(rit, it->second);
        }
        return result;
    }

private:
    const std::multimap<std::string, llmr::Value> values;
};


TEST(FilterComparison, SingleStringValue) {
    using namespace llmr;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::forward_list<Value> { std::string("bar") });
    // comparison is { "test": { "==": "bar" } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("bar") }, { "test", std::string("booz") }})));
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


TEST(FilterComparison, SingleDoubleValue) {
    using namespace llmr;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::forward_list<Value> { double(32.8) });
    // comparison is { "test": { "==": 32.8 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(32.8) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("32.8") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(32.8) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", double(32.8) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", double(32.8) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(32.9) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
}

TEST(FilterComparison, SingleUintValue) {
    using namespace llmr;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::forward_list<Value> { uint64_t(42) });
    // comparison is { "test": { "==": 42 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("42") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(42) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", uint64_t(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", uint64_t(42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(43) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
}

TEST(FilterComparison, SingleIntValue) {
    using namespace llmr;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::forward_list<Value> { int64_t(-42) });
    // comparison is { "test": { "==": -42 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(-42) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("-42") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(-42) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", int64_t(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", int64_t(-42) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-43) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", double(-43) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(-42) }})));
}

TEST(FilterComparison, SingleBoolValue) {
    using namespace llmr;

    FilterComparison comparison("test");
    comparison.add(FilterComparison::Operator::Equal, std::forward_list<Value> { bool(true) });
    // comparison is { "test": { "==": 32.8 } }

    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", int64_t(1) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", uint64_t(1) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", double(1) }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", std::string("1") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "test", bool(true) }, { "test", std::string("booz") }})));
    ASSERT_TRUE(comparison.compare(MockExtractor({{ "other", std::string("bar") }, { "test", bool(true) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "other", bool(true) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor()));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", bool(false) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", int64_t(-18932) }})));
    ASSERT_FALSE(comparison.compare(MockExtractor({{ "test", uint64_t(18932) }})));
}
