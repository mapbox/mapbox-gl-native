#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {

bool Filter::operator()(expression::EvaluationContext context) const {
    return FilterBase::visit(*this, FilterEvaluator { context });
}

bool Filter::operator()(const GeometryTileFeature& feature) const {
    return operator()(expression::EvaluationContext { &feature });
}

} // namespace style
} // namespace mbgl
