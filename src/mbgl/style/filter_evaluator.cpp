#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {

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

bool FilterEvaluator::operator()(const ExpressionFilter& filter) const {
    // TODO What is our fallback behvaiour?
    if (!filter.expression) return true;
    
    const expression::EvaluationResult result = filter.expression->evaluate(context);
    if (result) {
        const optional<bool> typed = expression::fromExpressionValue<bool>(*result);
        return typed ? *typed : false;
    }
    return false;
}
    
} // namespace style
} // namespace mbgl
