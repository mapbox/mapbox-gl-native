#include <mbgl/map/vector_tile.hpp>
#include <mbgl/geometry/pbf_geometry.hpp>
#include <mbgl/style/filter_expression_private.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/pbf.hpp>

#include <type_traits>
#include <ostream>

using namespace mbgl;

VectorTileFeature::VectorTileFeature(pbf feature_pbf, const GeometryTileLayer& layer) {
    while (feature_pbf.next()) {
        if (feature_pbf.tag == 1) { // id
            id = feature_pbf.varint<uint64_t>();
        } else if (feature_pbf.tag == 2) { // tags
            // tags are packed varints. They should have an even length.
            pbf tags = feature_pbf.message();
            while (tags) {
                uint32_t tag_key = tags.varint();

                if (layer.getKeys().size() <= tag_key) {
                    throw std::runtime_error("feature referenced out of range key");
                }

                if (tags) {
                    uint32_t tag_val = tags.varint();
                    if (layer.getValues().size() <= tag_val) {
                        throw std::runtime_error("feature referenced out of range value");
                    }

                    properties.emplace(layer.getKeys()[tag_key], layer.getValues()[tag_val]);
                } else {
                    throw std::runtime_error("uneven number of feature tag ids");
                }
            }
        } else if (feature_pbf.tag == 3) { // type
            type = (GeometryFeatureType)feature_pbf.varint();
        } else if (feature_pbf.tag == 4) { // geometry
            geometry_pbf = feature_pbf.message();
        } else {
            feature_pbf.skip();
        }
    }
}

GeometryCollection VectorTileFeature::nextGeometry() {

    GeometryCollection result;

    while (geometry_pbf.next(4)) { // geometry
        pbf current_geometry_pbf = geometry_pbf.message();
        PBFGeometry current_geometry(current_geometry_pbf);
        PBFGeometry::command cmd;
        int32_t x, y;

        if (type == GeometryFeatureType::Point) {
            if ((cmd = current_geometry.next(x, y)) != PBFGeometry::end) {
                GeometryPoint point(x, y);
                result.emplace_back(GeometryPoint(x, y));
            }
        } else if (type == GeometryFeatureType::LineString) {
            GeometryLine line;
            while ((cmd = current_geometry.next(x, y)) != PBFGeometry::end) {
                if (cmd == PBFGeometry::move_to) {
                    if (!line.empty()) {
                        result.push_back(line);
                        line.clear();
                    }
                }
                line.emplace_back(x, y);
            }
            if (line.size()) {
                result.push_back(line);
            }
        } else if (type == GeometryFeatureType::Polygon) {
            GeometryLine line;
            while ((cmd = current_geometry.next(x, y)) != PBFGeometry::end) {
                if (cmd == PBFGeometry::move_to) {
                    if (line.size()) {
                        result.push_back(line);
                        line.clear();
                    }
                }
                line.emplace_back(x, y);
            }

            if (line.size()) {
                result.push_back(line);
                line.clear();
            }
        } else {
            throw std::runtime_error("unrecognized geometry type");
        }
    }

    return std::move(result);
}

bool VectorTileFeature::operator==(const VectorTileFeature& other) const {
    return (geometry_pbf == other.geometry_pbf);
}

VectorTile::VectorTile(pbf tile_pbf) {
    while (tile_pbf.next()) {
        if (tile_pbf.tag == 3) { // layer
            VectorTileLayer layer(tile_pbf.message());
            layers.emplace(layer.getName(), std::forward<VectorTileLayer>(layer));
        } else {
            tile_pbf.skip();
        }
    }
}

VectorTile& VectorTile::operator=(VectorTile&& other) {
    if (this != &other) {
        layers.swap(other.layers);
    }
    return *this;
}

VectorTileLayer::VectorTileLayer(pbf layer_pbf)
    : feature_pbf(layer_pbf) {
    while (layer_pbf.next()) {
        if (layer_pbf.tag == 1) { // name
            name = layer_pbf.string();
        } else if (layer_pbf.tag == 3) { // keys
            keys.emplace_back(layer_pbf.string());
            key_index.emplace(keys.back(), keys.size() - 1);
        } else if (layer_pbf.tag == 4) { // values
            values.emplace_back(std::move(parseValue(layer_pbf.message())));
        } else if (layer_pbf.tag == 5) { // extent
            extent = layer_pbf.varint();
        } else {
            layer_pbf.skip();
        }
    }
}

std::unique_ptr<GeometryFilteredTileLayer> VectorTileLayer::createFilteredTileLayer(const FilterExpression& filterExpression) const {
    return util::make_unique<FilteredVectorTileLayer>(*this, filterExpression);
}

FilteredVectorTileLayer::FilteredVectorTileLayer(const VectorTileLayer& layer_, const FilterExpression& filterExpression_)
    : layer(layer_),
      filterExpression(filterExpression_) {}

GeometryFilteredTileLayer::iterator FilteredVectorTileLayer::begin() const {
    return iterator(*this, layer.feature_pbf);
}

GeometryFilteredTileLayer::iterator FilteredVectorTileLayer::end() const {
    return iterator(*this, pbf(layer.feature_pbf.end, 0));
}

FilteredVectorTileLayer::iterator::iterator(const FilteredVectorTileLayer& parent_, const pbf& feature_pbf_)
    : parent(parent_),
      feature(VectorTileFeature(pbf(), parent_.layer)),
      feature_pbf(feature_pbf_) {
    operator++();
}

template <>
GeometryTileTagExtractor<pbf>::GeometryTileTagExtractor(const GeometryTileLayer& layer_)
    : layer(layer_) {}

template <>
mapbox::util::optional<Value> GeometryTileTagExtractor<pbf>::getValue(const std::string& key) const {
    if (key == "$type") {
        return Value(uint64_t(type));
    }

    mapbox::util::optional<Value> value;

    auto field_it = layer.getKeyIndex().find(key);
    if (field_it != layer.getKeyIndex().end()) {
        const uint32_t filter_key = field_it->second;

        // Now loop through all the key/value pair tags.
        // tags are packed varints. They should have an even length.
        pbf tags_pbf_ = tags;
        uint32_t tag_key, tag_val;
        while (tags_pbf_) {
            tag_key = tags_pbf_.varint();
            if (!tags_pbf_) {
                // This should not happen; otherwise the vector tile is invalid.
                fprintf(stderr, "[WARNING] uneven number of feature tag ids\n");
                return value;
            }
            // Note: We need to run this command in all cases, even if the keys don't match.
            tag_val = tags_pbf_.varint();

            if (tag_key == filter_key) {
                if (layer.getValues().size() > tag_val) {
                    value = layer.getValues()[tag_val];
                } else {
                    fprintf(stderr, "[WARNING] feature references out of range value\n");
                    break;
                }
            }
        }
    }

    return value;
}

template bool mbgl::evaluate(const FilterExpression&, const GeometryTileTagExtractor<pbf>&);

void FilteredVectorTileLayer::iterator::operator++() {
    valid = false;

    const FilterExpression &expression = parent.filterExpression;

    while (feature_pbf.next(2)) { // feature
        feature = VectorTileFeature(feature_pbf.message(), parent.layer);
        pbf feature_pbf_ = feature_pbf.message();

        GeometryTileTagExtractor<pbf> extractor(parent.layer);

        // Retrieve the basic information
        while (feature_pbf_.next()) {
            if (feature_pbf_.tag == 2) { // tags
                extractor.setTags(feature_pbf_.message());
            } else if (feature_pbf_.tag == 3) { // geometry type
                extractor.setType(GeometryFeatureType(feature_pbf_.varint()));
            } else {
                feature_pbf_.skip();
            }
        }

        if (evaluate(expression, extractor)) {
            valid = true;
            return; // data loop
        } else {
            valid = false;
        }
    }
}

bool FilteredVectorTileLayer::iterator::operator!=(const iterator& other) const {
    return !(feature == other.feature);
}

const VectorTileFeature& FilteredVectorTileLayer::iterator::operator*() const {
    return feature;
}
