#pragma once

#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

using TileCoordinatePoint = Point<double>;

// Has floating point x/y coordinates.
// Used for computing the tiles that need to be visible in the viewport.
class TileCoordinate {
public:
    TileCoordinatePoint p;
    double z;

    static TileCoordinate fromLatLng(const TransformState& state, double zoom, const LatLng& latLng) {
        const double scale = std::pow(2, zoom - state.getZoom());
        return { state.project(latLng) * scale / double(util::tileSize), zoom };
    }

    static TileCoordinate fromScreenCoordinate(const TransformState& state, double zoom, const ScreenCoordinate& screenCoordinate) {
        return fromLatLng(state, zoom, state.screenCoordinateToLatLng(screenCoordinate));
    }

    TileCoordinate zoomTo(double zoom) const {
        return { p * std::pow(2, zoom - z), zoom };
    }

    static GeometryCoordinate toGeometryCoordinate(const UnwrappedTileID& tileID, const TileCoordinatePoint& point) {
        auto zoomed = TileCoordinate { point, 0 }.zoomTo(tileID.canonical.z);
        return {
            int16_t(util::clamp<int64_t>((zoomed.p.x - tileID.canonical.x - tileID.wrap * std::pow(2.0, tileID.canonical.z)) * util::EXTENT,
                        std::numeric_limits<int16_t>::min(),
                        std::numeric_limits<int16_t>::max())),
            int16_t(util::clamp<int64_t>((zoomed.p.y - tileID.canonical.y) * util::EXTENT,
                        std::numeric_limits<int16_t>::min(),
                        std::numeric_limits<int16_t>::max()))
        };
    }
};

} // namespace mbgl
