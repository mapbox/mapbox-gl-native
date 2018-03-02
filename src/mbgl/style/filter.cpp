#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {

bool Filter::operator()(const expression::EvaluationContext &context) const {
    return FilterBase::visit(*this, FilterEvaluator { context });
}

} // namespace style
} // namespace mbgl
