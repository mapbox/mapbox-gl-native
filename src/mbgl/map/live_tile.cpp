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

void LiveTileLayer::prepareToAddFeatures(size_t count) {
    features.reserve(features.size() + count);
}

void LiveTileLayer::addFeature(util::ptr<const LiveTileFeature> feature) {
    features.push_back(std::move(feature));
}

void LiveTileLayer::removeFeature(util::ptr<const LiveTileFeature> feature) {
    for (auto it = features.begin(); it != features.end(); ++it) {
        if (feature == *it) {
            features.erase(it);
            return;
        }
    }
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
