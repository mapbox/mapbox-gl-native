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

};

} // namespace style
} // namespace mbgl
