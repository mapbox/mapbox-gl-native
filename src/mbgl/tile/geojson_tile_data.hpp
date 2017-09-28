#include <mbgl/tile/geometry_tile_data.hpp>

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

    optional<FeatureIdentifier> getID() const override {
        return feature.id;
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
    GeoJSONTileLayer(std::shared_ptr<const mapbox::geometry::feature_collection<int16_t>> features_)
        : features(std::move(features_)) {
    }

    std::size_t featureCount() const override {
        return features->size();
    }

    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override {
        return std::make_unique<GeoJSONTileFeature>((*features)[i]);
    }

    std::string getName() const override {
        return "";
    }

private:
    std::shared_ptr<const mapbox::geometry::feature_collection<int16_t>> features;
};

class GeoJSONTileData : public GeometryTileData {
public:
    GeoJSONTileData(mapbox::geometry::feature_collection<int16_t> features_)
        : features(std::make_shared<mapbox::geometry::feature_collection<int16_t>>(
              std::move(features_))) {
    }

    GeoJSONTileData(std::shared_ptr<const mapbox::geometry::feature_collection<int16_t>> features_)
        : features(std::move(features_)) {
    }

    std::unique_ptr<GeometryTileData> clone() const override {
        return std::make_unique<GeoJSONTileData>(features);
    }

    std::unique_ptr<GeometryTileLayer> getLayer(const std::string&) const override {
        return std::make_unique<GeoJSONTileLayer>(features);
    }


private:
    std::shared_ptr<const mapbox::geometry::feature_collection<int16_t>> features;
};

} // namespace mbgl
