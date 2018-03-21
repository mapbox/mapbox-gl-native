#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

VectorTileFeature::VectorTileFeature(const vtzero::layer& layer_,
                                     const vtzero::feature& feature_)
    : feature(feature_), layer(layer_) {
}

FeatureType VectorTileFeature::getType() const {
    switch (feature.geometry_type()) {
    case vtzero::GeomType::POINT:
        return FeatureType::Point;
    case vtzero::GeomType::LINESTRING:
        return FeatureType::LineString;
    case vtzero::GeomType::POLYGON:
        return FeatureType::Polygon;
    default:
        return FeatureType::Unknown;
    }
}

struct PropertyValueMapping : vtzero::property_value_mapping {
    using float_type = double;
};

optional<Value> VectorTileFeature::getValue(const std::string& key) const {
    optional<Value> result;
    feature.for_each_property([&](vtzero::property&& p) {
        if (p.key() == key) {
            result = vtzero::convert_property_value<Value, PropertyValueMapping>(p.value());
        }
        return !result;
    });
    return result;
}

std::unordered_map<std::string, Value> VectorTileFeature::getProperties() const {
    std::unordered_map<std::string, Value> map;
    feature.for_each_property([&](const vtzero::property& p) {
        map.emplace(std::string(p.key()), vtzero::convert_property_value<Value, PropertyValueMapping>(p.value()));
        return true;
    });
    return map;
}

optional<FeatureIdentifier> VectorTileFeature::getID() const {
    return {feature.id()};
}

// Decode every geometry into a common structure: GeometryCollection, aka
// std::vector<std::vector<mapbox::geometry::Point<int16_t>>>
struct GeometryHandler {
    GeometryHandler(const float scale_) : scale(scale_) {}
    
    void begin(uint32_t count) {
        // cap allocation to 1MB
        constexpr std::uint32_t MAX_LENGTH = (1024 * 1024) / 16;
        if (count > MAX_LENGTH) {
            count = MAX_LENGTH;
        }
        result_.emplace_back();
        result_.back().reserve(count);
    }
    
    void add_point(vtzero::point point) {
        float scaled_x = ::roundf(static_cast<float>(point.x) * scale);
        float scaled_y = ::roundf(static_cast<float>(point.y) * scale);
        static const float max_coord = static_cast<float>(std::numeric_limits<typename GeometryCollection::coordinate_type>::max());
        static const float min_coord = static_cast<float>(std::numeric_limits<typename GeometryCollection::coordinate_type>::min());

        if (scaled_x > max_coord ||
            scaled_x < min_coord ||
            scaled_y > max_coord ||
            scaled_y < min_coord
        ) {
            std::runtime_error("paths outside valid range of coordinate_type");
        } else {
            result_.back().emplace_back(
                static_cast<typename GeometryCollection::coordinate_type>(scaled_x),
                static_cast<typename GeometryCollection::coordinate_type>(scaled_y));
        }
    }
    
    // decode_point_geometry handlers
    void points_begin(uint32_t count) { begin(count); }
    void points_point(vtzero::point point) { add_point(point); }
    void points_end() {}

    // decode_linestring_geometry handlers
    void linestring_begin(uint32_t count) { begin(count); }
    void linestring_point(vtzero::point point) { add_point(point); }
    void linestring_end() {}
    
    // decode_polygon_geometry handlers
    void ring_begin(uint32_t count) { begin(count); }
    void ring_point(vtzero::point point) { add_point(point); }
    void ring_end(vtzero::ring_type) {}

    GeometryCollection result() {
        return result_;
    }
    
    const float scale;
    GeometryCollection result_;
};

GeometryCollection VectorTileFeature::getGeometries() const {
    const float scale = float(util::EXTENT) / layer.extent();
    const vtzero::geometry geometry = feature.geometry();
    GeometryCollection lines = vtzero::decode_geometry(geometry, GeometryHandler(scale));
    if (layer.version() >= 2 || feature.geometry_type() != vtzero::GeomType::POLYGON) {
        return lines;
    } else {
        return fixupPolygons(lines);
    }
}

VectorTileLayer::VectorTileLayer(std::shared_ptr<const std::string> data_,
                                 const vtzero::layer& layer_)
    : data(std::move(data_)), layer(layer_) {
    layer.for_each_feature([&](vtzero::feature&& f) {
        features.push_back(f);
        return true;
    });
}

std::size_t VectorTileLayer::featureCount() const {
    return layer.num_features();
}

std::unique_ptr<GeometryTileFeature> VectorTileLayer::getFeature(std::size_t i) const {
    return std::make_unique<VectorTileFeature>(layer, features[i]);
}

std::string VectorTileLayer::getName() const {
    return static_cast<std::string>(layer.name());
}

VectorTileData::VectorTileData(std::shared_ptr<const std::string> data_) :
    data(std::move(data_)),
    tile(*data)
{
}

std::unique_ptr<GeometryTileData> VectorTileData::clone() const {
    return std::make_unique<VectorTileData>(data);
}

std::unique_ptr<GeometryTileLayer> VectorTileData::getLayer(const std::string& name) const {
    parseLayers();
    auto it = layers.find(name);
    if (it != layers.end()) {
        return std::make_unique<VectorTileLayer>(data, it->second);
    }
    return nullptr;
}

std::vector<std::string> VectorTileData::layerNames() const {
    parseLayers();
    std::vector<std::string> names;
    names.reserve(layers.size());
    for (const auto& entry : layers) {
        names.emplace_back(entry.first);
    }
    return names;
}

void VectorTileData::parseLayers() const {
    if (!parsed) {
        // We're parsing this lazily so that we can construct VectorTileData objects on the main
        // thread without incurring the overhead of parsing immediately.
        tile.for_each_layer([&](vtzero::layer&& layer) {
            layers[static_cast<std::string>(layer.name())] = std::move(layer);
            return true;
        });
        parsed = true;
    }
}

} // namespace mbgl
