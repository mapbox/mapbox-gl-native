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
    const mapbox::geometry::feature<int16_t>& feature;

    GeoJSONTileFeature(const mapbox::geometry::feature<int16_t>& feature_)
        : feature(feature_) {
    }

    FeatureType getType() const override  {
        return apply_visitor(ToFeatureType(), feature.geometry);
    }

    PropertyMap getProperties() const override {
        return feature.properties;
    }

    GeometryCollection getGeometries() const override {
        GeometryCollection geometry = apply_visitor(ToGeometryCollection(), feature.geometry);

        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (getType() == FeatureType::Polygon) {
            geometry = fixupPolygons(geometry);
        }

        return geometry;
    }

    optional<Value> getValue(const std::string& key) const override {
        auto it = feature.properties.find(key);
        if (it != feature.properties.end()) {
            return optional<Value>(it->second);
        }
        return optional<Value>();
    }
};

class GeoJSONTileLayer : public GeometryTileLayer {
public:
    const mapbox::geometry::feature_collection<int16_t>& features;

    GeoJSONTileLayer(const mapbox::geometry::feature_collection<int16_t>& features_)
        : features(features_) {
    }

    std::string getName() const override {
        return "";
    }

    std::size_t featureCount() const override {
        return features.size();
    }

    util::ptr<const GeometryTileFeature> getFeature(std::size_t i) const override {
        return std::make_shared<GeoJSONTileFeature>(features[i]);
    }
};

class GeoJSONTileData : public GeometryTileData {
public:
    mapbox::geometry::feature_collection<int16_t> features;

    GeoJSONTileData(mapbox::geometry::feature_collection<int16_t> features_)
        : features(std::move(features_)) {
    }

    util::ptr<const GeometryTileLayer> getLayer(const std::string&) const override {
        // We're ignoring the layer name because GeoJSON tiles only have one layer.
        return std::make_shared<GeoJSONTileLayer>(features);
    }
};

GeoJSONTile::GeoJSONTile(const OverscaledTileID& overscaledTileID,
                         std::string sourceID_,
                         const style::UpdateParameters& parameters,
                         const mapbox::geometry::feature_collection<int16_t>& features)
    : GeometryTile(overscaledTileID, sourceID_, parameters.style, parameters.mode) {
    setData(std::make_unique<GeoJSONTileData>(features));
}

void GeoJSONTile::setNecessity(Necessity) {}

} // namespace mbgl
