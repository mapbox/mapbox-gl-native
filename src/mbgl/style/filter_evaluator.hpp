#pragma once

#include <mbgl/style/filter.hpp>
#include <mbgl/tile/geometry_tile.hpp>

#include <type_traits>

namespace mbgl {

class FilterEvaluator {
public:
    FilterEvaluator(const GeometryTileFeature& feature_)
        : feature(feature_) {}

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
    optional<Value> getValue(const std::string& key) const {
        return key == "$type"
            ? optional<Value>(uint64_t(feature.getType()))
            : feature.getValue(key);
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

        bool operator()(const std::vector<Value>&,
                        const std::vector<Value>&) const {
            return false;
        }

        bool operator()(const std::unordered_map<std::string, Value>&,
                        const std::unordered_map<std::string, Value>&) const {
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

    const GeometryTileFeature& feature;
};

} // namespace mbgl
