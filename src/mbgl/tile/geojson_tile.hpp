#ifndef MBGL_ANNOTATION_GEOJSON_VT_TILE
#define MBGL_ANNOTATION_GEOJSON_VT_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/tile_id.hpp>

#include <unordered_map>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

// Implements a simple in-memory Tile type that holds GeoJSON values. A GeoJSON tile can only have
// one layer, and it is always returned regardless of which layer is requested.

class GeoJSONTileFeature : public GeometryTileFeature {
public:
    using Tags = std::unordered_map<std::string, std::string>;

    GeoJSONTileFeature(FeatureType, GeometryCollection&&, Tags&& = Tags{});
    FeatureType getType() const override;
    mapbox::util::optional<Value> getValue(const std::string&) const override;
    GeometryCollection getGeometries() const override;

private:
    const FeatureType type;
    const GeometryCollection geometries;
    const Tags tags;
};

class GeoJSONTileLayer : public GeometryTileLayer {
public:
    using Features = std::vector<std::shared_ptr<const GeoJSONTileFeature>>;

    GeoJSONTileLayer(Features&&);
    std::size_t featureCount() const override;
    util::ptr<const GeometryTileFeature> getFeature(std::size_t) const override;

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

class GeoJSONTileMonitor : public GeometryTileMonitor {
public:
    GeoJSONTileMonitor(mapbox::geojsonvt::GeoJSONVT*, const TileID&);
    virtual ~GeoJSONTileMonitor();

    std::unique_ptr<FileRequest> monitorTile(const GeometryTileMonitor::Callback&) override;

    void setGeoJSONVT(mapbox::geojsonvt::GeoJSONVT*);

private:
    void update();

public:
    const TileID tileID;

private:
    mapbox::geojsonvt::GeoJSONVT* geojsonvt = nullptr;
    GeometryTileMonitor::Callback callback;
};

} // namespace mbgl

#endif
