#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>

#include <string>
#include <vector>
#include <tuple>

namespace mbgl {
namespace style {

class Filter;

class NullFilter {
public:
    friend bool operator==(const NullFilter&, const NullFilter&) {
        return true;
    }
};

class EqualsFilter {
public:
    std::string key;
    Value value;

    friend bool operator==(const EqualsFilter& lhs, const EqualsFilter& rhs) {
        return std::tie(lhs.key, lhs.value) == std::tie(rhs.key, rhs.value);
    }
};

class NotEqualsFilter {
public:
    std::string key;
    Value value;

    friend bool operator==(const NotEqualsFilter& lhs, const NotEqualsFilter& rhs) {
        return std::tie(lhs.key, lhs.value) == std::tie(rhs.key, rhs.value);
    }
};

class LessThanFilter {
public:
    std::string key;
    Value value;

    friend bool operator==(const LessThanFilter& lhs, const LessThanFilter& rhs) {
        return std::tie(lhs.key, lhs.value) == std::tie(rhs.key, rhs.value);
    }
};

class LessThanEqualsFilter {
public:
    std::string key;
    Value value;

    friend bool operator==(const LessThanEqualsFilter& lhs, const LessThanEqualsFilter& rhs) {
        return std::tie(lhs.key, lhs.value) == std::tie(rhs.key, rhs.value);
    }
};

class GreaterThanFilter {
public:
    std::string key;
    Value value;

    friend bool operator==(const GreaterThanFilter& lhs, const GreaterThanFilter& rhs) {
        return std::tie(lhs.key, lhs.value) == std::tie(rhs.key, rhs.value);
    }
};

class GreaterThanEqualsFilter {
public:
    std::string key;
    Value value;

    friend bool operator==(const GreaterThanEqualsFilter& lhs, const GreaterThanEqualsFilter& rhs) {
        return std::tie(lhs.key, lhs.value) == std::tie(rhs.key, rhs.value);
    }
};

class InFilter {
public:
    std::string key;
    std::vector<Value> values;

    friend bool operator==(const InFilter& lhs, const InFilter& rhs) {
        return std::tie(lhs.key, lhs.values) == std::tie(rhs.key, rhs.values);
    }
};

class NotInFilter {
public:
    std::string key;
    std::vector<Value> values;

    friend bool operator==(const NotInFilter& lhs, const NotInFilter& rhs) {
        return std::tie(lhs.key, lhs.values) == std::tie(rhs.key, rhs.values);
    }
};

class AnyFilter {
public:
    std::vector<Filter> filters;

    friend bool operator==(const AnyFilter& lhs, const AnyFilter& rhs) {
        return lhs.filters == rhs.filters;
    }
};

class AllFilter {
public:
    std::vector<Filter> filters;

    friend bool operator==(const AllFilter& lhs, const AllFilter& rhs) {
        return lhs.filters == rhs.filters;
    }
};

class NoneFilter {
public:
    std::vector<Filter> filters;

    friend bool operator==(const NoneFilter& lhs, const NoneFilter& rhs) {
        return lhs.filters == rhs.filters;
    }
};

class HasFilter {
public:
    std::string key;

    friend bool operator==(const HasFilter& lhs, const HasFilter& rhs) {
        return lhs.key == rhs.key;
    }
};

class NotHasFilter {
public:
    std::string key;

    friend bool operator==(const NotHasFilter& lhs, const NotHasFilter& rhs) {
        return lhs.key == rhs.key;
    }
};


class TypeEqualsFilter {
public:
    FeatureType value;

    friend bool operator==(const TypeEqualsFilter& lhs, const TypeEqualsFilter& rhs) {
        return lhs.value == rhs.value;
    }
};

class TypeNotEqualsFilter {
public:
    FeatureType value;

    friend bool operator==(const TypeNotEqualsFilter& lhs, const TypeNotEqualsFilter& rhs) {
        return lhs.value == rhs.value;
    }
};

class TypeInFilter {
public:
    std::vector<FeatureType> values;

    friend bool operator==(const TypeInFilter& lhs, const TypeInFilter& rhs) {
        return lhs.values == rhs.values;
    }
};

class TypeNotInFilter {
public:
    std::vector<FeatureType> values;

    friend bool operator==(const TypeNotInFilter& lhs, const TypeNotInFilter& rhs) {
        return lhs.values == rhs.values;
    }
};


class IdentifierEqualsFilter {
public:
    FeatureIdentifier value;

    friend bool operator==(const IdentifierEqualsFilter& lhs, const IdentifierEqualsFilter& rhs) {
        return lhs.value == rhs.value;
    }
};

class IdentifierNotEqualsFilter {
public:
    FeatureIdentifier value;

    friend bool operator==(const IdentifierNotEqualsFilter& lhs, const IdentifierNotEqualsFilter& rhs) {
        return lhs.value == rhs.value;
    }
};

class IdentifierInFilter {
public:
    std::vector<FeatureIdentifier> values;

    friend bool operator==(const IdentifierInFilter& lhs, const IdentifierInFilter& rhs) {
        return lhs.values == rhs.values;
    }
};

class IdentifierNotInFilter {
public:
    std::vector<FeatureIdentifier> values;

    friend bool operator==(const IdentifierNotInFilter& lhs, const IdentifierNotInFilter& rhs) {
        return lhs.values == rhs.values;
    }
};

class HasIdentifierFilter {
public:
    friend bool operator==(const HasIdentifierFilter&, const HasIdentifierFilter&) {
        return true;
    }
};

class NotHasIdentifierFilter {
public:
    friend bool operator==(const NotHasIdentifierFilter&, const NotHasIdentifierFilter&) {
        return true;
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
    class NotHasFilter,
    class TypeEqualsFilter,
    class TypeNotEqualsFilter,
    class TypeInFilter,
    class TypeNotInFilter,
    class IdentifierEqualsFilter,
    class IdentifierNotEqualsFilter,
    class IdentifierInFilter,
    class IdentifierNotInFilter,
    class HasIdentifierFilter,
    class NotHasIdentifierFilter>;

class Filter : public FilterBase {
public:
    using FilterBase::FilterBase;

    bool operator()(const Feature&) const;

    template <class GeometryTileFeature>
    bool operator()(const GeometryTileFeature&) const;

    template <class PropertyAccessor>
    bool operator()(FeatureType type, optional<FeatureIdentifier> id, PropertyAccessor accessor) const;
};

} // namespace style
} // namespace mbgl
