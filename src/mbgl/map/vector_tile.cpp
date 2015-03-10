#include <mbgl/map/vector_tile.hpp>
#include <mbgl/geometry/pbf_geometry.hpp>
#include <mbgl/style/filter_expression_private.hpp>
#include <mbgl/util/pbf.hpp>

#include <type_traits>
#include <ostream>

using namespace mbgl;

VectorTileFeature::VectorTileFeature(pbf feature_pbf, const VectorTileLayer& layer) {
    while (feature_pbf.next()) {
        if (feature_pbf.tag == 1) { // id
            id = feature_pbf.varint<uint64_t>();
        } else if (feature_pbf.tag == 2) { // tags
            // tags are packed varints. They should have an even length.
            pbf tags = feature_pbf.message();
            while (tags) {
                uint32_t tag_key = tags.varint();

                if (layer.keys.size() <= tag_key) {
                    throw std::runtime_error("feature referenced out of range key");
                }

                if (tags) {
                    uint32_t tag_val = tags.varint();
                    if (layer.values.size() <= tag_val) {
                        throw std::runtime_error("feature referenced out of range value");
                    }

                    properties.emplace(layer.keys[tag_key], layer.values[tag_val]);
                } else {
                    throw std::runtime_error("uneven number of feature tag ids");
                }
            }
        } else if (feature_pbf.tag == 3) { // type
            type = (FeatureType)feature_pbf.varint();
        } else if (feature_pbf.tag == 4) { // geometry
            geometry_pbf = feature_pbf.message();
        } else {
            feature_pbf.skip();
        }
    }
}

mapbox::util::optional<Value> VectorTileFeature::getValue(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return mapbox::util::optional<Value>();
}

GeometryCollection VectorTileFeature::getGeometries() const {
    GeometryCollection result;

    PBFGeometry geometry(geometry_pbf);
    PBFGeometry::command cmd;
    int32_t x, y;

    std::vector<Coordinate> line;

    while ((cmd = geometry.next(x, y)) != PBFGeometry::end) {
        if (cmd == PBFGeometry::move_to) {
            if (!line.empty()) {
                result.push_back(line);
                line.clear();
            }
        }
        line.emplace_back(x, y);
    }

    if (!line.empty()) {
        result.push_back(line);
    }

    return std::move(result);
}

VectorTile::VectorTile(pbf tile_pbf) {
    while (tile_pbf.next()) {
        if (tile_pbf.tag == 3) { // layer
            util::ptr<VectorTileLayer> layer = std::make_shared<VectorTileLayer>(tile_pbf.message());
            layers.emplace(layer->name, layer);
        } else {
            tile_pbf.skip();
        }
    }
}

const util::ptr<const GeometryTileLayer> VectorTile::getLayer(const std::string& name) const {
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
            keys.emplace_back(layer_pbf.string());
        } else if (layer_pbf.tag == 4) { // values
            values.emplace_back(std::move(parseValue(layer_pbf.message())));
        } else if (layer_pbf.tag == 5) { // extent
            extent = layer_pbf.varint();
        } else {
            layer_pbf.skip();
        }
    }
}

const util::ptr<const GeometryTileFeature> VectorTileLayer::feature(std::size_t i) const {
    return std::make_shared<VectorTileFeature>(features.at(i), *this);
}
