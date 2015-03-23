#ifndef MBGL_MAP_LIVE_TILE
#define MBGL_MAP_LIVE_TILE

#include <map>
#include <unordered_map>

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

    std::size_t addFeature(util::ptr<const LiveTileFeature>);
    void removeFeature(std::size_t i) { features.erase(i); }
    std::size_t featureCount() const override { return features.size(); }
    util::ptr<const GeometryTileFeature> getFeature(std::size_t) const override;

private:
    std::size_t nextID() { return nextID_++; }

private:
    std::unordered_map<std::size_t, util::ptr<const LiveTileFeature>> features;
    std::size_t nextID_ = 0;
};

class LiveTile : public GeometryTile {
public:
    LiveTile();

    void addLayer(const std::string&, util::ptr<LiveTileLayer>);
    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;
    util::ptr<LiveTileLayer> getMutableLayer(const std::string&) const;
    bool operator()(const LiveTile&) const { return layers.size() > 0; }

private:
    std::map<std::string, util::ptr<LiveTileLayer>> layers;
};

}

#endif
