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
    
class Filter {
public:
    optional<std::shared_ptr<const expression::Expression>> expression;
    
    Filter() : expression() {}
    
    Filter(expression::ParseResult _expression) : expression(std::move(*_expression)) {
        assert(!expression || *expression != nullptr);
    }
    
    bool operator()(const expression::EvaluationContext& context) const;

    friend bool operator==(const Filter& lhs, const Filter& rhs) {
        if (!lhs.expression || !rhs.expression) {
            return lhs.expression == rhs.expression;
        } else {
            return *(lhs.expression) == *(rhs.expression);
        }
    }
    
    friend bool operator!=(const Filter& lhs, const Filter& rhs) {
        return !(lhs == rhs);
    }
};

} // namespace style
} // namespace mbgl
