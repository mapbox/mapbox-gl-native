#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/source/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/url.hpp>

#include <utility>

namespace mbgl {

Value parseValue(pbf data) {
    while (data.next())
    {
        switch (data.tag)
        {
        case 1: // string_value
            return data.string();
        case 2: // float_value
            return static_cast<double>(data.float32());
        case 3: // double_value
            return data.float64();
        case 4: // int_value
            return data.varint<int64_t>();
        case 5: // uint_value
            return data.varint<uint64_t>();
        case 6: // sint_value
            return data.svarint<int64_t>();
        case 7: // bool_value
            return data.boolean();
        default:
            data.skip();
            break;
        }
    }
    return false;
}

VectorTileFeature::VectorTileFeature(pbf feature_pbf, const VectorTileLayer& layer_)
    : layer(layer_) {
    while (feature_pbf.next()) {
        if (feature_pbf.tag == 1) { // id
            id = feature_pbf.varint<uint64_t>();
        } else if (feature_pbf.tag == 2) { // tags
            tags_pbf = feature_pbf.message();
        } else if (feature_pbf.tag == 3) { // type
            type = (FeatureType)feature_pbf.varint();
        } else if (feature_pbf.tag == 4) { // geometry
            geometry_pbf = feature_pbf.message();
        } else {
            feature_pbf.skip();
        }
    }
}

optional<Value> VectorTileFeature::getValue(const std::string& key) const {
    auto keyIter = layer.keys.find(key);
    if (keyIter == layer.keys.end()) {
        return optional<Value>();
    }

    pbf tags = tags_pbf;
    while (tags) {
        uint32_t tag_key = tags.varint();

        if (layer.keys.size() <= tag_key) {
            throw std::runtime_error("feature referenced out of range key");
        }

        if (!tags) {
            throw std::runtime_error("uneven number of feature tag ids");
        }

        uint32_t tag_val = tags.varint();
        if (layer.values.size() <= tag_val) {
            throw std::runtime_error("feature referenced out of range value");
        }

        if (tag_key == keyIter->second) {
            return layer.values[tag_val];
        }
    }

    return optional<Value>();
}

GeometryCollection VectorTileFeature::getGeometries() const {
    pbf data(geometry_pbf);
    uint8_t cmd = 1;
    uint32_t length = 0;
    int32_t x = 0;
    int32_t y = 0;

    GeometryCollection lines;

    lines.emplace_back();
    std::vector<Coordinate>* line = &lines.back();

    while (data.data < data.end) {
        if (length == 0) {
            uint32_t cmd_length = data.varint();
            cmd = cmd_length & 0x7;
            length = cmd_length >> 3;
        }

        --length;

        if (cmd == 1 || cmd == 2) {
            x += data.svarint();
            y += data.svarint();

            if (cmd == 1 && !line->empty()) { // moveTo
                lines.emplace_back();
                line = &lines.back();
            }

            line->emplace_back(x, y);

        } else if (cmd == 7) { // closePolygon
            if (!line->empty()) {
                line->push_back((*line)[0]);
            }

        } else {
            throw std::runtime_error("unknown command");
        }
    }

    return lines;
}

uint32_t VectorTileFeature::getExtent() const {
    return layer.extent;
}

VectorTile::VectorTile(std::shared_ptr<const std::string> data_)
    : data(std::move(data_)) {
}

util::ptr<GeometryTileLayer> VectorTile::getLayer(const std::string& name) const {
    if (!parsed) {
        parsed = true;
        pbf tile_pbf(reinterpret_cast<const unsigned char *>(data->c_str()), data->size());
        while (tile_pbf.next()) {
            if (tile_pbf.tag == 3) { // layer
                util::ptr<VectorTileLayer> layer = std::make_shared<VectorTileLayer>(tile_pbf.message());
                layers.emplace(layer->name, layer);
            } else {
                tile_pbf.skip();
            }
        }
    }

    auto layer_it = layers.find(name);
    if (layer_it != layers.end()) {
        return layer_it->second;
    }

    return nullptr;
}

VectorTileLayer::VectorTileLayer(pbf layer_pbf) {
    while (layer_pbf.next()) {
        if (layer_pbf.tag == 1) { // name
            name = layer_pbf.string();
        } else if (layer_pbf.tag == 2) { // feature
            features.push_back(layer_pbf.message());
        } else if (layer_pbf.tag == 3) { // keys
            keys.emplace(layer_pbf.string(), keys.size());
        } else if (layer_pbf.tag == 4) { // values
            values.emplace_back(parseValue(layer_pbf.message()));
        } else if (layer_pbf.tag == 5) { // extent
            extent = layer_pbf.varint();
        } else {
            layer_pbf.skip();
        }
    }
}

util::ptr<const GeometryTileFeature> VectorTileLayer::getFeature(std::size_t i) const {
    return std::make_shared<VectorTileFeature>(features.at(i), *this);
}

VectorTileMonitor::VectorTileMonitor(const TileID& tileID_, float pixelRatio_, const std::string& urlTemplate_)
    : tileID(tileID_),
      pixelRatio(pixelRatio_),
      urlTemplate(urlTemplate_) {
}

std::unique_ptr<FileRequest> VectorTileMonitor::monitorTile(const GeometryTileMonitor::Callback& callback) {
    const Resource resource = Resource::tile(urlTemplate, pixelRatio, tileID.x, tileID.y, tileID.sourceZ);
    return util::ThreadContext::getFileSource()->request(resource, [callback, this](Response res) {
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
