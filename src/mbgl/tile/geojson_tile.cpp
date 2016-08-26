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
    GeoJSONTileFeature(FeatureType type_, GeometryCollection&& geometries_, PropertyMap&& properties_)
        : type(type_), geometries(std::move(geometries_)), properties(std::move(properties_)) {
    }

    FeatureType getType() const override  {
        return type;
    }

    PropertyMap getProperties() const override {
        return properties;
    }

    GeometryCollection getGeometries() const override {
        return geometries;
    }

    optional<Value> getValue(const std::string& key) const override {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return optional<Value>(it->second);
        }
        return optional<Value>();
    }

private:
    const FeatureType type;
    const GeometryCollection geometries;
    const PropertyMap properties;
};

class GeoJSONTileLayer : public GeometryTileLayer {
public:
    std::vector<std::shared_ptr<const GeoJSONTileFeature>> features;

    std::string getName() const override {
        return "";
    }

    std::size_t featureCount() const override {
        return features.size();
    }

    util::ptr<const GeometryTileFeature> getFeature(std::size_t i) const override {
        return features[i];
    }
};

class GeoJSONTileData : public GeometryTileData {
public:
    std::shared_ptr<GeoJSONTileLayer> layer = std::make_shared<GeoJSONTileLayer>();

    GeoJSONTileData(const mapbox::geometry::feature_collection<int16_t>& features) {
        for (auto& feature : features) {
            const FeatureType featureType = apply_visitor(ToFeatureType(), feature.geometry);

            if (featureType == FeatureType::Unknown) {
                continue;
            }

            GeometryCollection geometry = apply_visitor(ToGeometryCollection(), feature.geometry);

            // https://github.com/mapbox/geojson-vt-cpp/issues/44
            if (featureType == FeatureType::Polygon) {
                geometry = fixupPolygons(geometry);
            }

            layer->features.emplace_back(std::make_shared<GeoJSONTileFeature>(
                featureType, std::move(geometry), PropertyMap(feature.properties)));
        }
    }

    util::ptr<const GeometryTileLayer> getLayer(const std::string&) const override {
        // We're ignoring the layer name because GeoJSON tiles only have one layer.
        return layer;
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
