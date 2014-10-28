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

struct NullExpression {
    template <class Extractor>
    bool evaluate(const Extractor&) const { return true; }
};

struct EqualsExpression {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct NotEqualsExpression {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct LessThanExpression {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct LessThanEqualsExpression {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct GreaterThanExpression {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct GreaterThanEqualsExpression {
    std::string key;
    Value value;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct InExpression {
    std::string key;
    std::vector<Value> values;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct NotInExpression {
    std::string key;
    std::vector<Value> values;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct AnyExpression {
    std::vector<FilterExpression> expressions;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct AllExpression {
    std::vector<FilterExpression> expressions;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

struct NoneExpression {
    std::vector<FilterExpression> expressions;

    template <class Extractor>
    bool evaluate(const Extractor&) const;
};

}

#endif
