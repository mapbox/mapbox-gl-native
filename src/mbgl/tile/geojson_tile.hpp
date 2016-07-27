#pragma once

#include <mbgl/tile/geometry_tile.hpp>

namespace mapbox {

namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt

namespace supercluster {
class Supercluster;
} // namespace supercluster

} // namespace mapbox

namespace mbgl {

namespace style {
class UpdateParameters;
} // namespace style

class GeoJSONTile : public GeometryTile {
public:
    GeoJSONTile(const OverscaledTileID&,
                std::string sourceID,
                const style::UpdateParameters&,
                mapbox::geojsonvt::GeoJSONVT&);

    GeoJSONTile(const OverscaledTileID&,
                std::string sourceID,
                const style::UpdateParameters&,
                mapbox::supercluster::Supercluster&);

    void setNecessity(Necessity) final;
};

} // namespace mbgl
