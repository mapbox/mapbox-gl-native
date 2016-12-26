#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>

#include <string>
#include <vector>

namespace mbgl {
namespace style {

class Filter;

class NullFilter {
public:
    bool operator==(const NullFilter&) const {
        return true;
    }
};

class EqualsFilter {
public:
    std::string key;
    Value value;

    bool operator==(const EqualsFilter& other) const {
        return key == other.key && value == other.value;
    }
};

class NotEqualsFilter {
public:
    std::string key;
    Value value;

    bool operator==(const NotEqualsFilter& other) const {
        return key == other.key && value == other.value;
    }
};

class LessThanFilter {
public:
    std::string key;
    Value value;

    bool operator==(const LessThanFilter& other) const {
        return key == other.key && value == other.value;
    }
};

class LessThanEqualsFilter {
public:
    std::string key;
    Value value;

    bool operator==(const LessThanEqualsFilter& other) const {
        return key == other.key && value == other.value;
    }
};

class GreaterThanFilter {
public:
    std::string key;
    Value value;

    bool operator==(const GreaterThanFilter& other) const {
        return key == other.key && value == other.value;
    }
};

class GreaterThanEqualsFilter {
public:
    std::string key;
    Value value;

    bool operator==(const GreaterThanEqualsFilter& other) const {
        return key == other.key && value == other.value;
    }
};

class InFilter {
public:
    std::string key;
    std::vector<Value> values;

    bool operator==(const InFilter& other) const {
        return key == other.key && values == other.values;
    }
};

class NotInFilter {
public:
    std::string key;
    std::vector<Value> values;

    bool operator==(const NotInFilter& other) const {
        return key == other.key && values == other.values;
    }
};

class AnyFilter {
public:
    std::vector<Filter> filters;

    bool operator==(const AnyFilter& other) const {
        return filters == other.filters;
    }
};

class AllFilter {
public:
    std::vector<Filter> filters;

    bool operator==(const AllFilter& other) const {
        return filters == other.filters;
    }
};

class NoneFilter {
public:
    std::vector<Filter> filters;

    bool operator==(const NoneFilter& other) const {
        return filters == other.filters;
    }
};

class HasFilter {
public:
    std::string key;

    bool operator==(const HasFilter& other) const {
        return key == other.key;
    }
};

class NotHasFilter {
public:
    std::string key;

    bool operator==(const NotHasFilter& other) const {
        return key == other.key;
    }
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
