#include <mbgl/util/get_geometries.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/constants.hpp>

#include <cmath>

namespace mbgl {

// the minimum position we can store in the buffers
constexpr int32_t EXTENT_MIN = util::EXTENT * -2;
// // the maximum position we can store in the buffers
constexpr int32_t EXTENT_MAX = (util::EXTENT * 2) - 1;

GeometryCollection getGeometries(const GeometryTileFeature& feature) {
    const float scale = float(util::EXTENT) / feature.getExtent();
    GeometryCollection geometryCollection = feature.getGeometries();
    for (auto& line : geometryCollection) {
        for (auto& point : line) {
            point.x = ::round(point.x * scale);
            point.y = ::round(point.y * scale);
            if (point.x < EXTENT_MIN ||
                point.x > EXTENT_MAX ||
                point.y < EXTENT_MIN ||
                point.y > EXTENT_MAX) {
                Log::Warning(Event::ParseTile, "Geometry exceeds allowed extent, reduce your vector tile buffer size");
            }
        }
    }
    return geometryCollection;
}

} // namespace mbgl

