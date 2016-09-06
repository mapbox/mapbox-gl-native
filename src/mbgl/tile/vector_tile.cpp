#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <protozero/pbf_reader.hpp>

#include <unordered_map>
#include <unordered_map>
#include <functional>
#include <utility>

namespace mbgl {

class VectorTileLayer;

using packed_iter_type = protozero::iterator_range<protozero::pbf_reader::const_uint32_iterator>;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(protozero::pbf_reader, const VectorTileLayer&);

    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string&) const override;
    std::unordered_map<std::string,Value> getProperties() const override;
    optional<FeatureIdentifier> getID() const override;
    GeometryCollection getGeometries() const override;

private:
    const VectorTileLayer& layer;
    optional<FeatureIdentifier> id;
    FeatureType type = FeatureType::Unknown;
    packed_iter_type tags_iter;
    packed_iter_type geometry_iter;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(protozero::pbf_reader);

    std::size_t featureCount() const override { return features.size(); }
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t) const override;
    std::string getName() const override;

private:
    friend class VectorTileData;
    friend class VectorTileFeature;

    std::string name;
    uint32_t version = 1;
    uint32_t extent = 4096;
    std::unordered_map<std::string, uint32_t> keysMap;
    std::vector<std::reference_wrapper<const std::string>> keys;
    std::vector<Value> values;
    std::vector<protozero::pbf_reader> features;
};

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(std::shared_ptr<const std::string> data);

    std::unique_ptr<GeometryTileData> clone() const override {
        return std::make_unique<VectorTileData>(*this);
    }

    const GeometryTileLayer* getLayer(const std::string&) const override;

private:
    std::shared_ptr<const std::string> data;
    mutable bool parsed = false;
    mutable std::unordered_map<std::string, VectorTileLayer> layers;
};

VectorTile::VectorTile(const OverscaledTileID& id_,
                       std::string sourceID_,
                       const style::UpdateParameters& parameters,
                       const Tileset& tileset)
    : GeometryTile(id_, sourceID_, parameters),
      loader(*this, id_, parameters, tileset) {
}

void VectorTile::setNecessity(Necessity necessity) {
    loader.setNecessity(necessity);
}

void VectorTile::setData(std::shared_ptr<const std::string> data_,
                         optional<Timestamp> modified_,
                         optional<Timestamp> expires_) {
    modified = modified_;
    expires = expires_;

    GeometryTile::setData(data_ ? std::make_unique<VectorTileData>(data_) : nullptr);
}

Value parseValue(protozero::pbf_reader data) {
    while (data.next())
    {
        switch (data.tag()) {
        case 1: // string_value
            return data.get_string();
        case 2: // float_value
            return static_cast<double>(data.get_float());
        case 3: // double_value
            return data.get_double();
        case 4: // int_value
            return data.get_int64();
        case 5: // uint_value
            return data.get_uint64();
        case 6: // sint_value
            return data.get_sint64();
        case 7: // bool_value
            return data.get_bool();
        default:
            data.skip();
            break;
        }
    }
    return false;
}

VectorTileFeature::VectorTileFeature(protozero::pbf_reader feature_pbf, const VectorTileLayer& layer_)
    : layer(layer_) {
    while (feature_pbf.next()) {
        switch (feature_pbf.tag()) {
        case 1: // id
            id = { feature_pbf.get_uint64() };
            break;
        case 2: // tags
            tags_iter = feature_pbf.get_packed_uint32();
            break;
        case 3: // type
            type = static_cast<FeatureType>(feature_pbf.get_enum());
            break;
        case 4: // geometry
            geometry_iter = feature_pbf.get_packed_uint32();
            break;
        default:
            feature_pbf.skip();
            break;
        }
    }
}

optional<Value> VectorTileFeature::getValue(const std::string& key) const {
    auto keyIter = layer.keysMap.find(key);
    if (keyIter == layer.keysMap.end()) {
        return optional<Value>();
    }

    auto start_itr = tags_iter.begin();
    const auto & end_itr = tags_iter.end();
    while (start_itr != end_itr) {
        uint32_t tag_key = static_cast<uint32_t>(*start_itr++);

        if (layer.keysMap.size() <= tag_key) {
            throw std::runtime_error("feature referenced out of range key");
        }

        if (start_itr == end_itr) {
            throw std::runtime_error("uneven number of feature tag ids");
        }

        uint32_t tag_val = static_cast<uint32_t>(*start_itr++);;
        if (layer.values.size() <= tag_val) {
            throw std::runtime_error("feature referenced out of range value");
        }

        if (tag_key == keyIter->second) {
            return layer.values[tag_val];
        }
    }

    return optional<Value>();
}

std::unordered_map<std::string,Value> VectorTileFeature::getProperties() const {
    std::unordered_map<std::string,Value> properties;
    auto start_itr = tags_iter.begin();
    const auto & end_itr = tags_iter.end();
    while (start_itr != end_itr) {
        uint32_t tag_key = static_cast<uint32_t>(*start_itr++);
        if (start_itr == end_itr) {
            throw std::runtime_error("uneven number of feature tag ids");
        }
        uint32_t tag_val = static_cast<uint32_t>(*start_itr++);
        properties[layer.keys.at(tag_key)] = layer.values.at(tag_val);
    }
    return properties;
}

optional<FeatureIdentifier> VectorTileFeature::getID() const {
    return id;
}

GeometryCollection VectorTileFeature::getGeometries() const {
    uint8_t cmd = 1;
    uint32_t length = 0;
    int32_t x = 0;
    int32_t y = 0;
    const float scale = float(util::EXTENT) / layer.extent;

    GeometryCollection lines;

    lines.emplace_back();
    GeometryCoordinates* line = &lines.back();

    auto g_itr = geometry_iter.begin();
    while (g_itr != geometry_iter.end()) {
        if (length == 0) {
            uint32_t cmd_length = static_cast<uint32_t>(*g_itr++);
            cmd = cmd_length & 0x7;
            length = cmd_length >> 3;
        }

        --length;

        if (cmd == 1 || cmd == 2) {
            x += protozero::decode_zigzag32(static_cast<uint32_t>(*g_itr++));
            y += protozero::decode_zigzag32(static_cast<uint32_t>(*g_itr++));

            if (cmd == 1 && !line->empty()) { // moveTo
                lines.emplace_back();
                line = &lines.back();
            }

            line->emplace_back(::round(x * scale), ::round(y * scale));

        } else if (cmd == 7) { // closePolygon
            if (!line->empty()) {
                line->push_back((*line)[0]);
            }

        } else {
            throw std::runtime_error("unknown command");
        }
    }

    if (layer.version >= 2 || type != FeatureType::Polygon) {
        return lines;
    }

    return fixupPolygons(lines);
}

VectorTileData::VectorTileData(std::shared_ptr<const std::string> data_)
    : data(std::move(data_)) {
}

const GeometryTileLayer* VectorTileData::getLayer(const std::string& name) const {
    if (!parsed) {
        parsed = true;
        protozero::pbf_reader tile_pbf(*data);
        while (tile_pbf.next(3)) {
            VectorTileLayer layer(tile_pbf.get_message());
            layers.emplace(layer.name, std::move(layer));
        }
    }

    auto it = layers.find(name);
    if (it != layers.end()) {
        return &it->second;
    }
    return nullptr;
}

VectorTileLayer::VectorTileLayer(protozero::pbf_reader layer_pbf) {
    while (layer_pbf.next()) {
        switch (layer_pbf.tag()) {
        case 1: // name
            name = layer_pbf.get_string();
            break;
        case 2: // feature
            features.push_back(layer_pbf.get_message());
            break;
        case 3: // keys
            {
                auto iter = keysMap.emplace(layer_pbf.get_string(), keysMap.size());
                keys.emplace_back(std::reference_wrapper<const std::string>(iter.first->first));
            }
            break;
        case 4: // values
            values.emplace_back(parseValue(layer_pbf.get_message()));
            break;
        case 5: // extent
            extent = layer_pbf.get_uint32();
            break;
        case 15: // version
            version = layer_pbf.get_uint32();
            break;
        default:
            layer_pbf.skip();
            break;
        }
    }
}

std::unique_ptr<GeometryTileFeature> VectorTileLayer::getFeature(std::size_t i) const {
    return std::make_unique<VectorTileFeature>(features.at(i), *this);
}

std::string VectorTileLayer::getName() const {
    return name;
}

} // namespace mbgl
