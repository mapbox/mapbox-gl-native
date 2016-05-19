#include <mbgl/tile/geojson_tile.hpp>

namespace mbgl {

GeoJSONTileFeature::GeoJSONTileFeature(FeatureType type_,
                                       GeometryCollection&& geometries_,
                                       Feature::property_map&& properties_)
    : type(type_), geometries(std::move(geometries_)), properties(std::move(properties_)) {
}

FeatureType GeoJSONTileFeature::getType() const {
    return type;
}

optional<Value> GeoJSONTileFeature::getValue(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return optional<Value>(it->second);
    }
    return optional<Value>();
}

GeometryCollection GeoJSONTileFeature::getGeometries() const {
    return geometries;
}

GeoJSONTileLayer::GeoJSONTileLayer(Features&& features_) : features(std::move(features_)) {
}

std::size_t GeoJSONTileLayer::featureCount() const {
    return features.size();
}

util::ptr<const GeometryTileFeature> GeoJSONTileLayer::getFeature(std::size_t i) const {
    return features[i];
}

GeoJSONTile::GeoJSONTile(std::shared_ptr<GeoJSONTileLayer> layer_) : layer(std::move(layer_)) {
}

util::ptr<GeometryTileLayer> GeoJSONTile::getLayer(const std::string&) const {
    // We're ignoring the layer name because GeoJSON tiles only have one layer.
    return layer;
}

} // namespace mbgl
