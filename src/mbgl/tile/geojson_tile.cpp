#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/update_parameters.hpp>

#include <mapbox/geojsonvt.hpp>
#include <supercluster.hpp>

namespace mbgl {

// Implements a simple in-memory Tile type that holds GeoJSON values. A GeoJSON tile can only have
// one layer, and it is always returned regardless of which layer is requested.

class GeoJSONTileFeature : public GeometryTileFeature {
public:
    GeoJSONTileFeature(FeatureType, GeometryCollection&&, PropertyMap&&);
    FeatureType getType() const override;
    optional<Value> getValue(const std::string&) const override;
    PropertyMap getProperties() const override { return properties; }
    GeometryCollection getGeometries() const override;

private:
    const FeatureType type;
    const GeometryCollection geometries;
    const PropertyMap properties;
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

class GeoJSONTileData : public GeometryTileData {
public:
    GeoJSONTileData(std::shared_ptr<GeoJSONTileLayer>);
    util::ptr<const GeometryTileLayer> getLayer(const std::string&) const override;

private:
    const std::shared_ptr<GeoJSONTileLayer> layer;
};

// Converts the geojsonvt::Tile to a a GeoJSONTile. They have a differing internal structure.
std::unique_ptr<GeoJSONTileData> convertTile(const mapbox::geometry::feature_collection<int16_t>& features) {
    std::shared_ptr<GeoJSONTileLayer> layer;

    if (!features.empty()) {
        std::vector<std::shared_ptr<const GeoJSONTileFeature>> convertedFeatures;

        ToFeatureType toFeatureType;
        ToGeometryCollection toGeometryCollection;

        for (auto& feature : features) {
            const FeatureType featureType = apply_visitor(toFeatureType, feature.geometry);

            if (featureType == FeatureType::Unknown) {
                continue;
            }

            GeometryCollection geometry = apply_visitor(toGeometryCollection, feature.geometry);

            // https://github.com/mapbox/geojson-vt-cpp/issues/44
            if (featureType == FeatureType::Polygon) {
                geometry = fixupPolygons(geometry);
            }

            PropertyMap properties = feature.properties;

            convertedFeatures.emplace_back(std::make_shared<GeoJSONTileFeature>(
                featureType, std::move(geometry), std::move(properties)));
        }

        layer = std::make_unique<GeoJSONTileLayer>(std::move(convertedFeatures));
    }

    return std::make_unique<GeoJSONTileData>(layer);
}

GeoJSONTile::GeoJSONTile(const OverscaledTileID& overscaledTileID,
                         std::string sourceID_,
                         const style::UpdateParameters& parameters,
                         mapbox::geojsonvt::GeoJSONVT& geojsonvt)
    : GeometryTile(overscaledTileID, sourceID_, parameters.style, parameters.mode) {
    setData(convertTile(geojsonvt.getTile(id.canonical.z, id.canonical.x, id.canonical.y).features));
}

GeoJSONTile::GeoJSONTile(const OverscaledTileID& overscaledTileID,
                         std::string sourceID_,
                         const style::UpdateParameters& parameters,
                         mapbox::supercluster::Supercluster& supercluster)
    : GeometryTile(overscaledTileID, sourceID_, parameters.style, parameters.mode) {
    setData(convertTile(supercluster.getTile(id.canonical.z, id.canonical.x, id.canonical.y)));
}

void GeoJSONTile::setNecessity(Necessity) {}

GeoJSONTileFeature::GeoJSONTileFeature(FeatureType type_,
                                       GeometryCollection&& geometries_,
                                       PropertyMap&& properties_)
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

GeoJSONTileData::GeoJSONTileData(std::shared_ptr<GeoJSONTileLayer> layer_) : layer(std::move(layer_)) {
}

util::ptr<const GeometryTileLayer> GeoJSONTileData::getLayer(const std::string&) const {
    // We're ignoring the layer name because GeoJSON tiles only have one layer.
    return layer;
}

} // namespace mbgl
