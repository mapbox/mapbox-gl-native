#include <mbgl/util/get_geometries.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

GeometryCollection getGeometries(const GeometryTileFeature& feature) {
    const float scale = float(util::EXTENT) / feature.getExtent();
    GeometryCollection geometryCollection = feature.getGeometries();
    for (auto& line : geometryCollection) {
        for (auto& point : line) {
            point.x = std::round(point.x * scale);
            point.y = std::round(point.y * scale);
        }
    }
    return geometryCollection;
}

} // namespace mbgl

