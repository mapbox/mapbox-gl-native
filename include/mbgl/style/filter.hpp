#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/expression/expression.hpp>

#include <string>
#include <vector>
#include <tuple>

namespace mbgl {
namespace style {

class Filter;
    
class ExpressionFilter {
public:
    std::shared_ptr<const expression::Expression> expression;

    friend bool operator==(const ExpressionFilter& lhs, const ExpressionFilter& rhs) {
        if (!lhs.expression || !rhs.expression) {
            return ((bool) lhs.expression) == ((bool) rhs.expression);
        } else {
            return *(lhs.expression) == *(rhs.expression);
        }
    }
};

using FilterBase = variant<class ExpressionFilter>;

class Filter : public FilterBase {
public:
    using FilterBase::FilterBase;
    bool operator()(const expression::EvaluationContext& context) const;
};

} // namespace style
} // namespace mbgl
