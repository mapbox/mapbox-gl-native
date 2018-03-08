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

template <class Op>
bool compare(const Value& lhs, const Value& rhs, const Op& op) {
    return Value::binary_visit(lhs, rhs, Comparator<Op> { op });
}

bool equal(const Value& lhs, const Value& rhs) {
    return compare(lhs, rhs, [] (const auto& lhs_, const auto& rhs_) { return lhs_ == rhs_; });
}

bool FilterEvaluator::operator()(const NullFilter&) const {
    return true;
}

bool FilterEvaluator::operator()(const EqualsFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    return actual && equal(*actual, filter.value);
}

bool FilterEvaluator::operator()(const NotEqualsFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    return !actual || !equal(*actual, filter.value);
}

bool FilterEvaluator::operator()(const LessThanFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ < rhs_; });
}

bool FilterEvaluator::operator()(const LessThanEqualsFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ <= rhs_; });
}

bool FilterEvaluator::operator()(const GreaterThanFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ > rhs_; });
}

bool FilterEvaluator::operator()(const GreaterThanEqualsFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    return actual && compare(*actual, filter.value, [] (const auto& lhs_, const auto& rhs_) { return lhs_ >= rhs_; });
}

bool FilterEvaluator::operator()(const InFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    if (!actual)
        return false;
    for (const auto& v: filter.values) {
        if (equal(*actual, v)) {
            return true;
        }
    }
    return false;
}

bool FilterEvaluator::operator()(const NotInFilter& filter) const {
    optional<Value> actual = context.feature->getValue(filter.key);
    if (!actual)
        return true;
    for (const auto& v: filter.values) {
        if (equal(*actual, v)) {
            return false;
        }
    }
    return true;
}

bool FilterEvaluator::operator()(const AnyFilter& filter) const {
    for (const auto& f: filter.filters) {
        if (Filter::visit(f, *this)) {
            return true;
        }
    }
    return false;
}

bool FilterEvaluator::operator()(const AllFilter& filter) const {
    for (const auto& f: filter.filters) {
        if (!Filter::visit(f, *this)) {
            return false;
        }
    }
    return true;
}

bool FilterEvaluator::operator()(const NoneFilter& filter) const {
    for (const auto& f: filter.filters) {
        if (Filter::visit(f, *this)) {
            return false;
        }
    }
    return true;
}

bool FilterEvaluator::operator()(const HasFilter& filter) const {
    return bool(context.feature->getValue(filter.key));
}

bool FilterEvaluator::operator()(const NotHasFilter& filter) const {
    return !context.feature->getValue(filter.key);
}

bool FilterEvaluator::operator()(const TypeEqualsFilter& filter) const {
    return context.feature->getType() == filter.value;
}

bool FilterEvaluator::operator()(const TypeNotEqualsFilter& filter) const {
    return context.feature->getType() != filter.value;
}

bool FilterEvaluator::operator()(const TypeInFilter& filter) const {
    for (const auto& v: filter.values) {
        if (context.feature->getType() == v) {
            return true;
        }
    }
    return false;
}

bool FilterEvaluator::operator()(const TypeNotInFilter& filter) const {
    for (const auto& v: filter.values) {
        if (context.feature->getType() == v) {
            return false;
        }
    }
    return true;
}

bool FilterEvaluator::operator()(const IdentifierEqualsFilter& filter) const {
    return context.feature->getID() == filter.value;
}

bool FilterEvaluator::operator()(const IdentifierNotEqualsFilter& filter) const {
    return context.feature->getID() != filter.value;
}

bool FilterEvaluator::operator()(const IdentifierInFilter& filter) const {
    for (const auto& v: filter.values) {
        if (context.feature->getID() == v) {
            return true;
        }
    }
    return false;
}

bool FilterEvaluator::operator()(const IdentifierNotInFilter& filter) const {
    for (const auto& v: filter.values) {
        if (context.feature->getID() == v) {
            return false;
        }
    }
    return true;
}

bool FilterEvaluator::operator()(const HasIdentifierFilter&) const {
    return bool(context.feature->getID());
}

bool FilterEvaluator::operator()(const NotHasIdentifierFilter&) const {
    return !context.feature->getID();
}

bool FilterEvaluator::operator()(const ExpressionFilter& filter) const {
    const expression::EvaluationResult result = filter.expression->evaluate(context);
    if (result) {
        const optional<bool> typed = expression::fromExpressionValue<bool>(*result);
        return typed ? *typed : false;
    }
    return false;
}
    
} // namespace style
} // namespace mbgl
