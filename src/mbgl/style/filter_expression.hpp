#ifndef MBGL_STYLE_FILTER_EXPRESSION
#define MBGL_STYLE_FILTER_EXPRESSION

#include <mbgl/style/value.hpp>

#include <rapidjson/document.h>

#include <string>
#include <vector>

namespace mbgl {

typedef mapbox::util::variant<
    struct NullExpression,
    struct EqualsExpression,
    struct NotEqualsExpression,
    struct LessThanExpression,
    struct LessThanEqualsExpression,
    struct GreaterThanExpression,
    struct GreaterThanEqualsExpression,
    struct InExpression,
    struct NotInExpression,
    struct AnyExpression,
    struct AllExpression,
    struct NoneExpression
    > FilterExpression;

FilterExpression parseFilterExpression(const rapidjson::Value&);

template <class Extractor>
bool evaluate(const FilterExpression&, const Extractor&);

struct NullExpression final {
    template <class Extractor>
    bool evaluate(const Extractor&) const { return true; }
};

struct EqualsExpression final {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct NotEqualsExpression final {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct LessThanExpression final {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct LessThanEqualsExpression final {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct GreaterThanExpression final {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct GreaterThanEqualsExpression final {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct InExpression final {
    std::string key;
    std::vector<Value> values;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct NotInExpression final {
    std::string key;
    std::vector<Value> values;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct AnyExpression final {
    std::vector<FilterExpression> expressions;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct AllExpression final {
    std::vector<FilterExpression> expressions;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct NoneExpression final {
    std::vector<FilterExpression> expressions;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

} // namespace mbgl

#endif
