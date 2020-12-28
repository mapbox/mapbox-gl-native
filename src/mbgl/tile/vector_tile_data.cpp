#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

VectorTileFeature::VectorTileFeature(const mapbox::vector_tile::layer& layer,
                                     const protozero::data_view& view)
    : feature(view, layer) {
}

FeatureType VectorTileFeature::getType() const {
    switch (feature.getType()) {
    case mapbox::vector_tile::GeomType::POINT:
        return FeatureType::Point;
    case mapbox::vector_tile::GeomType::LINESTRING:
        return FeatureType::LineString;
    case mapbox::vector_tile::GeomType::POLYGON:
        return FeatureType::Polygon;
    default:
        return FeatureType::Unknown;
    }
}

optional<Value> VectorTileFeature::getValue(const std::string& key) const {
    const optional<Value> value(feature.getValue(key));
    return value->is<NullValue>() ? nullopt : value;
}

const PropertyMap& VectorTileFeature::getProperties() const {
    if (!properties) {
        properties = feature.getProperties();
    }
    return *properties;
}

FeatureIdentifier VectorTileFeature::getID() const {
    return feature.getID();
}

const GeometryCollection& VectorTileFeature::getGeometries() const {
    if (!lines) {
        const float scale = float(util::EXTENT) / feature.getExtent();
        lines = feature.getGeometries<GeometryCollection>(scale);
        if (feature.getVersion() < 2 && feature.getType() == mapbox::vector_tile::GeomType::POLYGON) {
            lines = fixupPolygons(*lines);
        }
    }
    return *lines;
}

VectorTileLayer::VectorTileLayer(std::shared_ptr<const std::string> data_,
                                 const protozero::data_view& view)
    : data(std::move(data_)), layer(view) {
}

std::size_t VectorTileLayer::featureCount() const {
    return layer.featureCount();
}

std::unique_ptr<GeometryTileFeature> VectorTileLayer::getFeature(std::size_t i) const {
    return std::make_unique<VectorTileFeature>(layer, layer.getFeature(i));
}

std::string VectorTileLayer::getName() const {
    return layer.getName();
}

VectorTileData::VectorTileData(std::shared_ptr<const std::string> data_) : data(std::move(data_)) {
}

std::unique_ptr<GeometryTileData> VectorTileData::clone() const {
    return std::make_unique<VectorTileData>(data);
}

std::unique_ptr<GeometryTileLayer> VectorTileData::getLayer(const std::string& name) const {
    if (!parsed) {
        // We're parsing this lazily so that we can construct VectorTileData objects on the main
        // thread without incurring the overhead of parsing immediately.
        layers = mapbox::vector_tile::buffer(*data).getLayers();
        parsed = true;
    }

    auto it = layers.find(name);
    if (it != layers.end()) {
        return std::make_unique<VectorTileLayer>(data, it->second);
    }
    return nullptr;
}

std::vector<std::string> VectorTileData::layerNames() const {
    return mapbox::vector_tile::buffer(*data).layerNames();
}

} // namespace mbgl
