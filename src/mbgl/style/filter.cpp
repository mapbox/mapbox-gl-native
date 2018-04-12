#include <mbgl/style/filter.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {

bool Filter::operator()(const expression::EvaluationContext &context) const {
    
    // TODO What is our fallback behvaiour?
    if (!this->expression) return true;
    
    const expression::EvaluationResult result = this->expression->evaluate(context);
    if (result) {
        const optional<bool> typed = expression::fromExpressionValue<bool>(*result);
        return typed ? *typed : false;
    }
    return false;
    
}

} // namespace style
} // namespace mbgl
