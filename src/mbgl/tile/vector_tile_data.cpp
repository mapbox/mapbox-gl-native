#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

Value parseValue(protozero::pbf_reader data) {
    Value value;
    while (data.next()) {
        switch (data.tag()) {
        case 1: // string_value
            value = data.get_string();
            break;
        case 2: // float_value
            value = static_cast<double>(data.get_float());
            break;
        case 3: // double_value
            value = data.get_double();
            break;
        case 4: // int_value
            value = data.get_int64();
            break;
        case 5: // uint_value
            value = data.get_uint64();
            break;
        case 6: // sint_value
            value = data.get_sint64();
            break;
        case 7: // bool_value
            value = data.get_bool();
            break;
        default:
            data.skip();
            break;
        }
    }
    return value;
}

VectorTileFeature::VectorTileFeature(protozero::pbf_reader feature_pbf, std::shared_ptr<VectorTileLayerData> layerData_)
    : layerData(std::move(layerData_)) {
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
    auto keyIter = layerData->keysMap.find(key);
    if (keyIter == layerData->keysMap.end()) {
        return optional<Value>();
    }

    auto start_itr = tags_iter.begin();
    const auto & end_itr = tags_iter.end();
    while (start_itr != end_itr) {
        auto tag_key = static_cast<uint32_t>(*start_itr++);

        if (layerData->keysMap.size() <= tag_key) {
            throw std::runtime_error("feature referenced out of range key");
        }

        if (start_itr == end_itr) {
            throw std::runtime_error("uneven number of feature tag ids");
        }

        auto tag_val = static_cast<uint32_t>(*start_itr++);;
        if (layerData->values.size() <= tag_val) {
            throw std::runtime_error("feature referenced out of range value");
        }

        if (tag_key == keyIter->second) {
            return layerData->values[tag_val];
        }
    }

    return optional<Value>();
}

std::unordered_map<std::string,Value> VectorTileFeature::getProperties() const {
    std::unordered_map<std::string,Value> properties;
    auto start_itr = tags_iter.begin();
    const auto & end_itr = tags_iter.end();
    while (start_itr != end_itr) {
        auto tag_key = static_cast<uint32_t>(*start_itr++);
        if (start_itr == end_itr) {
            throw std::runtime_error("uneven number of feature tag ids");
        }
        auto tag_val = static_cast<uint32_t>(*start_itr++);
        properties[layerData->keys.at(tag_key)] = layerData->values.at(tag_val);
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
    const float scale = float(util::EXTENT) / layerData->extent;

    GeometryCollection lines;

    lines.emplace_back();
    GeometryCoordinates* line = &lines.back();

    auto g_itr = geometry_iter.begin();
    while (g_itr != geometry_iter.end()) {
        if (length == 0) {
            auto cmd_length = static_cast<uint32_t>(*g_itr++);
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

    if (layerData->version >= 2 || type != FeatureType::Polygon) {
        return lines;
    }

    return fixupPolygons(lines);
}

VectorTileData::VectorTileData(std::shared_ptr<const std::string> data_)
    : data(std::move(data_)) {
}

const GeometryTileLayer* VectorTileData::getLayer(const std::string& name) const {
    if (!parsed) {
        parse();
    }

    auto it = layers.find(name);
    if (it != layers.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> VectorTileData::layerNames() const {
    if (!parsed) {
        parse();
    }

    std::vector<std::string> names;
    names.reserve(layers.size());
    for (auto const& layer : layers) {
        names.emplace_back(layer.first);
    }
    return names;
}

void VectorTileData::parse() const {
    parsed = true;
    layers.clear();
    protozero::pbf_reader tile_pbf(*data);
    while (tile_pbf.next(3)) {
        VectorTileLayer layer(tile_pbf.get_message(), data);
        layers.emplace(layer.name, std::move(layer));
    }
}

VectorTileLayerData::VectorTileLayerData(std::shared_ptr<const std::string> pbfData) :
    data(std::move(pbfData))
{}

VectorTileLayer::VectorTileLayer(protozero::pbf_reader layer_pbf, std::shared_ptr<const std::string> pbfData)
    : data(std::make_shared<VectorTileLayerData>(std::move(pbfData)))
{
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
                auto iter = data->keysMap.emplace(layer_pbf.get_string(), data->keysMap.size());
                data->keys.emplace_back(std::reference_wrapper<const std::string>(iter.first->first));
            }
            break;
        case 4: // values
            data->values.emplace_back(parseValue(layer_pbf.get_message()));
            break;
        case 5: // extent
            data->extent = layer_pbf.get_uint32();
            break;
        case 15: // version
            data->version = layer_pbf.get_uint32();
            break;
        default:
            layer_pbf.skip();
            break;
        }
    }
}

std::unique_ptr<GeometryTileFeature> VectorTileLayer::getFeature(std::size_t i) const {
    return std::make_unique<VectorTileFeature>(features.at(i), data);
}

std::string VectorTileLayer::getName() const {
    return name;
}

} // namespace mbgl
