#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/value.hpp>

#include <string>
#include <vector>

namespace mbgl {

typedef variant<
    class NullFilter,
    class EqualsFilter,
    class NotEqualsFilter,
    class LessThanFilter,
    class LessThanEqualsFilter,
    class GreaterThanFilter,
    class GreaterThanEqualsFilter,
    class InFilter,
    class NotInFilter,
    class AnyFilter,
    class AllFilter,
    class NoneFilter
    > Filter;

class NullFilter {};

class EqualsFilter {
public:
    std::string key;
    Value value;
};

class NotEqualsFilter {
public:
    std::string key;
    Value value;
};

class LessThanFilter {
public:
    std::string key;
    Value value;
};

class LessThanEqualsFilter {
public:
    std::string key;
    Value value;
};

class GreaterThanFilter {
public:
    std::string key;
    Value value;
};

class GreaterThanEqualsFilter {
public:
    std::string key;
    Value value;
};

class InFilter {
public:
    std::string key;
    std::vector<Value> values;
};

class NotInFilter {
public:
    std::string key;
    std::vector<Value> values;
};

class AnyFilter {
public:
    std::vector<Filter> filters;
};

class AllFilter {
public:
    std::vector<Filter> filters;
};

class NoneFilter {
public:
    std::vector<Filter> filters;
};

} // namespace mbgl
