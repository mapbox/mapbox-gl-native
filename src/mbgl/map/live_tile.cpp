#include <mbgl/map/live_tile.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

LiveTileFeature::LiveTileFeature(FeatureType type_, GeometryCollection geometries_, std::map<std::string, std::string> properties_)
    : type(type_),
      properties(properties_),
      geometries(geometries_) {}

mapbox::util::optional<Value> LiveTileFeature::getValue(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return mapbox::util::optional<Value>(it->second);
    }
    return mapbox::util::optional<Value>();
}

LiveTileLayer::LiveTileLayer() {}

std::size_t LiveTileLayer::addFeature(util::ptr<const LiveTileFeature> feature) {
    std::size_t i = nextID();
    features.emplace(i, std::move(feature));
    return i;
}

util::ptr<const GeometryTileFeature> LiveTileLayer::getFeature(std::size_t i) const {
    auto it = features.begin();
    std::advance(it, i);
    return it->second;
}

LiveTile::LiveTile() {}

void LiveTile::addLayer(const std::string& name, util::ptr<LiveTileLayer> layer) {
    layers.emplace(name, std::move(layer));
}

util::ptr<GeometryTileLayer> LiveTile::getLayer(const std::string& name) const {
    return getMutableLayer(name);
}

util::ptr<LiveTileLayer> LiveTile::getMutableLayer(const std::string& name) const {
    auto layer_it = layers.find(name);
    if (layer_it != layers.end()) {
        return layer_it->second;
    }
    return nullptr;
}

}
