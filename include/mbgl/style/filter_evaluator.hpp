#pragma once

#include <mbgl/style/filter.hpp>
#include <mbgl/util/geometry.hpp>

#include <type_traits>

namespace mbgl {
namespace style {

/*
   A visitor that evaluates a `Filter` for a given feature.

   Use via `Filter::operator()`. For example:

       if (filter(feature)) {
           // matches the filter
       } else {
           // does not match
       }
*/
template <class PropertyAccessor>
class FilterEvaluator {
public:
    const FeatureType featureType;
    const optional<FeatureIdentifier> featureIdentifier;
    const PropertyAccessor propertyAccessor;

    bool operator()(const NullFilter&) const {
        return true;
    }

    bool operator()(const EqualsFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        return actual && equal(*actual, filter.value);
    }

    bool operator()(const NotEqualsFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        return !actual || !equal(*actual, filter.value);
    }

    bool operator()(const LessThanFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ < rhs_; });
    }

    bool operator()(const LessThanEqualsFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ <= rhs_; });
    }

    bool operator()(const GreaterThanFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ > rhs_; });
    }

    bool operator()(const GreaterThanEqualsFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ >= rhs_; });
    }

    bool operator()(const InFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        if (!actual)
            return false;
        for (const auto& v: filter.values) {
            if (equal(*actual, v)) {
                return true;
            }
        }
        return false;
    }

    bool operator()(const NotInFilter& filter) const {
        optional<Value> actual = getValue(filter.key);
        if (!actual)
            return true;
        for (const auto& v: filter.values) {
            if (equal(*actual, v)) {
                return false;
            }
        }
        return true;
    }

    bool operator()(const AnyFilter& filter) const {
        for (const auto& f: filter.filters) {
            if (Filter::visit(f, *this)) {
                return true;
            }
        }
        return false;
    }

    bool operator()(const AllFilter& filter) const {
        for (const auto& f: filter.filters) {
            if (!Filter::visit(f, *this)) {
                return false;
            }
        }
        return true;
    }

    bool operator()(const NoneFilter& filter) const {
        for (const auto& f: filter.filters) {
            if (Filter::visit(f, *this)) {
                return false;
            }
        }
        return true;
    }

    bool operator()(const HasFilter& filter) const {
        return bool(getValue(filter.key));
    }

    bool operator()(const NotHasFilter& filter) const {
        return !getValue(filter.key);
    }

private:
    optional<Value> getValue(const std::string& key_) const {
        if (key_ == "$type") {
            return optional<Value>(uint64_t(featureType));
        } else if (key_ == "$id") {
            if (featureIdentifier) {
                return FeatureIdentifier::visit(*featureIdentifier, [] (auto id) {
                    return Value(std::move(id));
                });
            } else {
                return optional<Value>();
            }
        } else {
            return propertyAccessor(key_);
        }
    }

    template <class Op>
    struct Comparator {
        const Op& op;

        template <class T>
        bool operator()(const T& lhs, const T& rhs) const {
            return op(lhs, rhs);
        }

        template <class T0, class T1>
        auto operator()(const T0& lhs, const T1& rhs) const
            -> typename std::enable_if_t<std::is_arithmetic<T0>::value && !std::is_same<T0, bool>::value &&
                                         std::is_arithmetic<T1>::value && !std::is_same<T1, bool>::value, bool> {
            return op(double(lhs), double(rhs));
        }

        template <class T0, class T1>
        auto operator()(const T0&, const T1&) const
            -> typename std::enable_if_t<!std::is_arithmetic<T0>::value || std::is_same<T0, bool>::value ||
                                         !std::is_arithmetic<T1>::value || std::is_same<T1, bool>::value, bool> {
            return false;
        }

        bool operator()(const NullValue&,
                        const NullValue&) const {
            // Should be unreachable; null is not currently allowed by the style specification.
            assert(false);
            return false;
        }

        bool operator()(const std::vector<Value>&,
                        const std::vector<Value>&) const {
            // Should be unreachable; nested values are not currently allowed by the style specification.
            assert(false);
            return false;
        }

        bool operator()(const PropertyMap&,
                        const PropertyMap&) const {
            // Should be unreachable; nested values are not currently allowed by the style specification.
            assert(false);
            return false;
        }
    };

    template <class Op>
    bool compare(const Value& lhs, const Value& rhs, const Op& op) const {
        return Value::binary_visit(lhs, rhs, Comparator<Op> { op });
    }

    bool equal(const Value& lhs, const Value& rhs) const {
        return compare(lhs, rhs, [] (const auto& lhs_, const auto& rhs_) { return lhs_ == rhs_; });
    }
};

inline bool Filter::operator()(const Feature& feature) const {
    return operator()(apply_visitor(ToFeatureType(), feature.geometry), feature.id, [&] (const std::string& key) -> optional<Value> {
        auto it = feature.properties.find(key);
        if (it == feature.properties.end())
            return {};
        return it->second;
    });
}

template <class PropertyAccessor>
bool Filter::operator()(FeatureType type, optional<FeatureIdentifier> id, PropertyAccessor accessor) const {
    return FilterBase::visit(*this, FilterEvaluator<PropertyAccessor> { type, id, accessor });
}

} // namespace style
} // namespace mbgl
