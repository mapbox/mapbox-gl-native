#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {

optional<Value> GeometryTileFeatureExtractor::getValue(const std::string& key) const {
    if (key == "$type") {
        return Value(uint64_t(feature.getType()));
    }

    return feature.getValue(key);
}

} // namespace mbgl
