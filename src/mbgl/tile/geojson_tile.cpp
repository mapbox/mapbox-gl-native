#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/map/query.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

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

class GeoJSONTileData : public GeometryTileData,
                        public GeometryTileLayer {
public:
    mapbox::geometry::feature_collection<int16_t> features;

    GeoJSONTileData(mapbox::geometry::feature_collection<int16_t> features_)
        : features(std::move(features_)) {
    }

    std::unique_ptr<GeometryTileData> clone() const override {
        return std::make_unique<GeoJSONTileData>(*this);
    }

    const GeometryTileLayer* getLayer(const std::string&) const override {
        return this;
    }

    std::string getName() const override {
        return "";
    }

    std::size_t featureCount() const override {
        return features.size();
    }

    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override {
        return std::make_unique<GeoJSONTileFeature>(features[i]);
    }
};

GeoJSONTile::GeoJSONTile(const OverscaledTileID& overscaledTileID,
                         std::string sourceID_,
                         const TileParameters& parameters,
                         mapbox::geometry::feature_collection<int16_t> features)
    : GeometryTile(overscaledTileID, sourceID_, parameters,
                   *parameters.style.glyphAtlas,
                   *parameters.style.spriteAtlas) {
    updateData(std::move(features));
}

void GeoJSONTile::updateData(mapbox::geometry::feature_collection<int16_t> features) {
    setData(std::make_unique<GeoJSONTileData>(std::move(features)));
}

void GeoJSONTile::setNecessity(Necessity) {}
    
void GeoJSONTile::querySourceFeatures(
    std::vector<Feature>& result,
    const SourceQueryOptions& options) {
    
    // Ignore the sourceLayer, there is only one
    auto layer = getData()->getLayer({});
    
    if (layer) {
        auto featureCount = layer->featureCount();
        for (std::size_t i = 0; i < featureCount; i++) {
            auto feature = layer->getFeature(i);
            
            // Apply filter, if any
            if (options.filter && !(*options.filter)(*feature)) {
                continue;
            }
            
            result.push_back(convertFeature(*feature, id.canonical));
        }
    }
}

} // namespace mbgl
