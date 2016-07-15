#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

// Has floating point x/y coordinates.
// Used for computing the tiles that need to be visible in the viewport.
class TileCoordinate {
public:
    Point<double> p;
    double z;

    static TileCoordinate fromLatLng(const TransformState& state, double zoom, const LatLng& latLng) {
        const double scale = std::pow(2, zoom - state.getZoom());
        return { state.project(latLng) * scale / double(util::tileSize), zoom };
    }
    
    LatLng toLatLng(const TransformState& state) const {
        const double scale = std::pow(2, z - state.getZoom());
        return state.unproject(p * double(util::tileSize) / scale);
    }

    static TileCoordinate fromScreenCoordinate(const TransformState& state, double zoom, const ScreenCoordinate& point) {
        return fromLatLng(state, zoom, state.screenCoordinateToLatLng(point));
    }

    TileCoordinate zoomTo(double zoom) const {
        return { p * std::pow(2, zoom - z), zoom };
    }
};

} // namespace mbgl
