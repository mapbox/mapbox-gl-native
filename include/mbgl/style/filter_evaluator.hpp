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
class FilterEvaluator {
public:
    const expression::EvaluationContext context;

    bool operator()(const NullFilter&) const;
    bool operator()(const EqualsFilter& filter) const;
    bool operator()(const NotEqualsFilter& filter) const;
    bool operator()(const LessThanFilter& filter) const;
    bool operator()(const LessThanEqualsFilter& filter) const;
    bool operator()(const GreaterThanFilter& filter) const;
    bool operator()(const GreaterThanEqualsFilter& filter) const;
    bool operator()(const InFilter& filter) const;
    bool operator()(const NotInFilter& filter) const;
    bool operator()(const AnyFilter& filter) const;
    bool operator()(const AllFilter& filter) const;
    bool operator()(const NoneFilter& filter) const;
    bool operator()(const HasFilter& filter) const;
    bool operator()(const NotHasFilter& filter) const;
    bool operator()(const TypeEqualsFilter& filter) const;
    bool operator()(const TypeNotEqualsFilter& filter) const;
    bool operator()(const TypeInFilter& filter) const;
    bool operator()(const TypeNotInFilter& filter) const;
    bool operator()(const IdentifierEqualsFilter& filter) const;
    bool operator()(const IdentifierNotEqualsFilter& filter) const;
    bool operator()(const IdentifierInFilter& filter) const;
    bool operator()(const IdentifierNotInFilter& filter) const;
    bool operator()(const HasIdentifierFilter&) const;
    bool operator()(const NotHasIdentifierFilter&) const;
    bool operator()(const ExpressionFilter&) const;

private:
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

template <class GeometryTileFeature>
bool Filter::operator()(const GeometryTileFeature& feature) const {
    return operator()(expression::EvaluationContext { &feature });
}

} // namespace style
} // namespace mbgl
