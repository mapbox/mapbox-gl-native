#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/style/update_parameters.hpp>

#include <mapbox/geojsonvt.hpp>

namespace mbgl {

// Converts the geojsonvt::Tile to a a GeoJSONTile. They have a differing internal structure.
std::unique_ptr<GeoJSONTile> convertTile(const mapbox::geojsonvt::Tile& tile) {
    std::shared_ptr<GeoJSONTileLayer> layer;

    if (tile) {
        std::vector<std::shared_ptr<const GeoJSONTileFeature>> features;
        GeometryCoordinates line;

        for (auto& feature : tile.features) {
            const FeatureType featureType =
                (feature.type == mapbox::geojsonvt::TileFeatureType::Point
                     ? FeatureType::Point
                     : (feature.type == mapbox::geojsonvt::TileFeatureType::LineString
                            ? FeatureType::LineString
                            : (feature.type == mapbox::geojsonvt::TileFeatureType::Polygon
                                   ? FeatureType::Polygon
                                   : FeatureType::Unknown)));
            if (featureType == FeatureType::Unknown) {
                continue;
            }

            GeometryCollection geometry;

            // Flatten the geometry; GeoJSONVT distinguishes between a Points array and Rings array
            // (Points = GeoJSON types Point, MultiPoint, LineString)
            // (Rings = GeoJSON types MultiLineString, Polygon, MultiPolygon)
            // However, in Mapbox GL, we use one structure for both types, and just have one outer
            // element for Points.
            if (feature.tileGeometry.is<mapbox::geojsonvt::TilePoints>()) {
                line.clear();
                for (auto& point : feature.tileGeometry.get<mapbox::geojsonvt::TilePoints>()) {
                    line.emplace_back(point.x, point.y);
                }
                geometry.emplace_back(std::move(line));
            } else if (feature.tileGeometry.is<mapbox::geojsonvt::TileRings>()) {
                for (auto& ring : feature.tileGeometry.get<mapbox::geojsonvt::TileRings>()) {
                    line.clear();
                    for (auto& point : ring) {
                        line.emplace_back(point.x, point.y);
                    }
                    geometry.emplace_back(std::move(line));
                }
            }

            // https://github.com/mapbox/geojson-vt-cpp/issues/44
            if (featureType == FeatureType::Polygon) {
                geometry = fixupPolygons(geometry);
            }

            Feature::property_map properties{ feature.tags.begin(), feature.tags.end() };

            features.emplace_back(std::make_shared<GeoJSONTileFeature>(
                featureType, std::move(geometry), std::move(properties)));
        }

        layer = std::make_unique<GeoJSONTileLayer>(std::move(features));
    }

    return std::make_unique<GeoJSONTile>(layer);
}

GeoJSONTileData::GeoJSONTileData(const OverscaledTileID& overscaledTileID,
                                 std::string sourceID,
                                 const style::UpdateParameters& parameters,
                                 mapbox::geojsonvt::GeoJSONVT* geojsonvt)
    : GeometryTileData(overscaledTileID, sourceID, parameters.style, parameters.mode) {
    if (geojsonvt) {
        auto tile = convertTile(
            geojsonvt->getTile(id.canonical.z, id.canonical.x, id.canonical.y));
        setData(std::move(tile), {}, {});
    }
}

GeoJSONTileData::~GeoJSONTileData() = default;

void GeoJSONTileData::setNecessity(Necessity) {}

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
