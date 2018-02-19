#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {

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

bool FilterEvaluator::operator()(const ExpressionFilter&) const {
    return false;
}

bool Filter::operator()(expression::EvaluationContext context) const {
    return FilterBase::visit(*this, FilterEvaluator { context });
}
    
} // namespace style
} // namespace mbgl
