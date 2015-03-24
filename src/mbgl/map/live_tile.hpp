#ifndef MBGL_MAP_LIVE_TILE
#define MBGL_MAP_LIVE_TILE

#include <map>

#include <mbgl/map/geometry_tile.hpp>

namespace mbgl {

class LiveTileFeature : public GeometryTileFeature {
public:
    LiveTileFeature(FeatureType, GeometryCollection, std::map<std::string, std::string> properties = {{}});

    FeatureType getType() const override { return type; }
    mapbox::util::optional<Value> getValue(const std::string&) const override;
    GeometryCollection getGeometries() const override { return geometries; }

private:
    FeatureType type = FeatureType::Unknown;
    std::map<std::string, std::string> properties;
    GeometryCollection geometries;
};

class LiveTileLayer : public GeometryTileLayer {
public:
    LiveTileLayer();

    void prepareToAddFeatures(size_t count);
    void addFeature(util::ptr<const LiveTileFeature>);
    void removeFeature(util::ptr<const LiveTileFeature>);
    std::size_t featureCount() const override { return features.size(); }
    util::ptr<const GeometryTileFeature> getFeature(std::size_t i) const override { return features[i]; }

private:
    std::vector<util::ptr<const LiveTileFeature>> features;
};

class LiveTile : public GeometryTile {
public:
    LiveTile();

    void addLayer(const std::string&, util::ptr<LiveTileLayer>);
    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;
    util::ptr<LiveTileLayer> getMutableLayer(const std::string&) const;

private:
    std::map<std::string, util::ptr<LiveTileLayer>> layers;
};

}

#endif
