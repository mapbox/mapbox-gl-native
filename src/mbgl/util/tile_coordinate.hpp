#ifndef MBGL_UTIL_TILE_COORDINATE
#define MBGL_UTIL_TILE_COORDINATE

#include <mbgl/style/types.hpp>
#include <mbgl/map/transform_state.hpp>


namespace mbgl {

class TransformState;

// Has floating point x/y coordinates.
// Used for computing the tiles that need to be visible in the viewport.
class TileCoordinate {
public:
    double x, y, z;

    static TileCoordinate fromLatLng(const TransformState& state, double zoom, const LatLng& latLng) {
        const double scale = std::pow(2, zoom - state.getZoom());
        return {
            state.lngX(latLng.longitude) * scale / util::tileSize,
            state.latY(latLng.latitude) * scale / util::tileSize,
            zoom
        };
    }

    static TileCoordinate fromScreenCoordinate(const TransformState& state, double zoom, const ScreenCoordinate& point) {
        return fromLatLng(state, zoom, state.screenCoordinateToLatLng(point));
    }

    TileCoordinate zoomTo(double zoom) const {
        double scale = std::pow(2, zoom - z);
        return { x * scale, y * scale, zoom };
    }
};

} // namespace mbgl

#endif
