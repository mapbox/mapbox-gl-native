#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/url.hpp>

#include <utility>

namespace mbgl {

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
            id = feature_pbf.get_uint64();
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

    auto start_itr = tags_iter.first;
    const auto & end_itr = tags_iter.second;
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
    auto start_itr = tags_iter.first;
    const auto & end_itr = tags_iter.second;
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

optional<uint64_t> VectorTileFeature::getID() const {
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

    auto g_itr = geometry_iter;
    while (g_itr.first != g_itr.second) {
        if (length == 0) {
            uint32_t cmd_length = static_cast<uint32_t>(*g_itr.first++);
            cmd = cmd_length & 0x7;
            length = cmd_length >> 3;
        }

        --length;

        if (cmd == 1 || cmd == 2) {
            x += protozero::decode_zigzag32(static_cast<uint32_t>(*g_itr.first++));
            y += protozero::decode_zigzag32(static_cast<uint32_t>(*g_itr.first++));

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

VectorTile::VectorTile(std::shared_ptr<const std::string> data_)
    : data(std::move(data_)) {
}

util::ptr<GeometryTileLayer> VectorTile::getLayer(const std::string& name) const {
    if (!parsed) {
        parsed = true;
        protozero::pbf_reader tile_pbf(*data);
        while (tile_pbf.next(3)) {
            util::ptr<VectorTileLayer> layer = std::make_shared<VectorTileLayer>(tile_pbf.get_message());
            layers.emplace(layer->name, layer);
        }
    }

    auto layer_it = layers.find(name);
    if (layer_it != layers.end()) {
        return layer_it->second;
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

util::ptr<const GeometryTileFeature> VectorTileLayer::getFeature(std::size_t i) const {
    return std::make_shared<VectorTileFeature>(features.at(i), *this);
}

std::string VectorTileLayer::getName() const {
    return name;
}

VectorTileMonitor::VectorTileMonitor(const OverscaledTileID& tileID_, float pixelRatio_,
                                     const std::string& urlTemplate_, FileSource& fileSource_)
    : tileID(tileID_),
      pixelRatio(pixelRatio_),
      urlTemplate(urlTemplate_),
      fileSource(fileSource_) {
}

std::unique_ptr<AsyncRequest> VectorTileMonitor::monitorTile(const GeometryTileMonitor::Callback& callback) {
    const Resource resource = Resource::tile(urlTemplate, pixelRatio, tileID.canonical.x,
                                             tileID.canonical.y, tileID.canonical.z);
    return fileSource.request(resource, [callback, this](Response res) {
        if (res.error) {
            callback(std::make_exception_ptr(std::runtime_error(res.error->message)), nullptr, res.modified, res.expires);
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            callback(nullptr, nullptr, res.modified, res.expires);
        } else {
            callback(nullptr, std::make_unique<VectorTile>(res.data), res.modified, res.expires);
        }
    });
}

} // namespace mbgl
