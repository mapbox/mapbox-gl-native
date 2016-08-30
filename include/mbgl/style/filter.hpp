#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>

#include <string>
#include <vector>

namespace mbgl {
namespace style {

class Filter;

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

class HasFilter {
public:
    std::string key;
};

class NotHasFilter {
public:
    std::string key;
};

using FilterBase = variant<
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
    class NoneFilter,
    class HasFilter,
    class NotHasFilter>;

class Filter : public FilterBase {
public:
    using FilterBase::FilterBase;

    bool operator()(const Feature&) const;

    template <class PropertyAccessor>
    bool operator()(FeatureType type, optional<FeatureIdentifier> id, PropertyAccessor accessor) const;
};

} // namespace style
} // namespace mbgl
