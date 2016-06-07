#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

namespace style {
class UpdateParameters;
}

class GeoJSONTileData : public GeometryTileData {
public:
    GeoJSONTileData(const OverscaledTileID&,
                   std::string sourceID,
                   const style::UpdateParameters&,
                   mapbox::geojsonvt::GeoJSONVT*);
    ~GeoJSONTileData();

    void setNecessity(Necessity) final;
};

// Implements a simple in-memory Tile type that holds GeoJSON values. A GeoJSON tile can only have
// one layer, and it is always returned regardless of which layer is requested.

class GeoJSONTileFeature : public GeometryTileFeature {
public:
    GeoJSONTileFeature(FeatureType, GeometryCollection&&, Feature::property_map&&);
    FeatureType getType() const override;
    optional<Value> getValue(const std::string&) const override;
    Feature::property_map getProperties() const override { return properties; }
    GeometryCollection getGeometries() const override;

private:
    const FeatureType type;
    const GeometryCollection geometries;
    const Feature::property_map properties;
};

class GeoJSONTileLayer : public GeometryTileLayer {
public:
    using Features = std::vector<std::shared_ptr<const GeoJSONTileFeature>>;

    GeoJSONTileLayer(Features&&);
    std::size_t featureCount() const override;
    util::ptr<const GeometryTileFeature> getFeature(std::size_t) const override;
    std::string getName() const override { return ""; };

private:
    const Features features;
};

class GeoJSONTile : public GeometryTile {
public:
    GeoJSONTile(std::shared_ptr<GeoJSONTileLayer>);
    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;

private:
    const std::shared_ptr<GeoJSONTileLayer> layer;
};

} // namespace mbgl
