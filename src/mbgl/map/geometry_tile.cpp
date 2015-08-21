#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/filter_expression_private.hpp>

namespace mbgl {

mapbox::util::optional<Value> GeometryTileFeatureExtractor::getValue(const std::string& key) const {
    if (key == "$type") {
        return Value(uint64_t(feature.getType()));
    }

    return feature.getValue(key);
}

template bool evaluate(const FilterExpression&, const GeometryTileFeatureExtractor&);

}
