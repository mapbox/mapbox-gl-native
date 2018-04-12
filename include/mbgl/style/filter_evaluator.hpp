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

    bool operator()(const ExpressionFilter&) const;

};

} // namespace style
} // namespace mbgl
