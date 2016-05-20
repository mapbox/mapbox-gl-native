#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <unordered_map>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class GeoJSONTileSource : public GeometryTileSource {
public:
    GeoJSONTileSource(GeometryTileData&, mapbox::geojsonvt::GeoJSONVT*, const OverscaledTileID&);
    virtual ~GeoJSONTileSource();

    void setGeoJSONVT(mapbox::geojsonvt::GeoJSONVT*);

private:
    void update();

public:
    const OverscaledTileID tileID;

private:
    mapbox::geojsonvt::GeoJSONVT* geojsonvt = nullptr;
};

} // namespace mbgl
