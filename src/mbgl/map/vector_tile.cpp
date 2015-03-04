#include <mbgl/map/vector_tile.hpp>
#include <mbgl/geometry/pbf_geometry.hpp>
#include <mbgl/style/filter_expression_private.hpp>
#include <mbgl/util/variant_io.hpp>

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
            type = (GeometryFeatureType)feature_pbf.varint();
        } else if (feature_pbf.tag == 4) { // geometry
            geometry_pbf = feature_pbf.message();
        } else {
            feature_pbf.skip();
        }
    }
}

Geometry VectorTileFeature::nextGeometry() {
    pbf current_geometry_pbf = geometry_pbf.message();
    PBFGeometry current_geometry(current_geometry_pbf);
    int32_t x, y;
    Geometry result;
    result.set<std::false_type>();

    if (multigeometry || geometry_pbf.next(4)) {
        if (type == GeometryFeatureType::Point) {
            current_geometry.next(x, y);
            GeometryPoint point(x, y);
            result.set<GeometryPoint>(point);
        } else if (type == GeometryFeatureType::LineString) {
            GeometryLine line;
            PBFGeometry::command cmd;
            while ((cmd = current_geometry.next(x, y)) != PBFGeometry::end) {
                if (cmd == PBFGeometry::move_to) {
                    multigeometry = true;
                    result.set<GeometryLine>(line);
                    break;
                } else {
                    line.emplace_back(x, y);
                }
            }
            if (!multigeometry) {
                result.set<GeometryLine>(line);
            }
        } else if (type == GeometryFeatureType::Polygon) {
            GeometryPolygon polygon;
            PBFGeometry::command cmd;
            while ((cmd = current_geometry.next(x, y)) != PBFGeometry::end) {
                if (cmd == PBFGeometry::move_to) {
                    multigeometry = true;
                    result.set<GeometryPolygon>(polygon);
                    break;
                } else {
//                    polygon.emplace_back(x, y);
                }
            }
            if (!multigeometry) {
                result.set<GeometryPolygon>(polygon);
            }
        } else {
            throw std::runtime_error("unrecognized geometry type");
        }
    }

    return std::move(result);
}

VectorTile::VectorTile(pbf tile) {
    while (tile.next()) {
        if (tile.tag == 3) { // layer
            VectorTileLayer layer(tile.message());
            layers.emplace(layer.name, std::forward<VectorTileLayer>(layer));
        } else {
            tile.skip();
        }
    }
}

VectorTileLayer::VectorTileLayer(pbf layer) : feature_pbf(layer) {
    while (feature_pbf.next()) {
        if (feature_pbf.tag == 1) { // name
            name = feature_pbf.string();
        } else if (feature_pbf.tag == 3) { // keys
            keys.emplace_back(feature_pbf.string());
            key_index.emplace(keys.back(), keys.size() - 1);
        } else if (feature_pbf.tag == 4) { // values
            values.emplace_back(std::move(parseValue(feature_pbf.message())));
        } else if (feature_pbf.tag == 5) { // extent
            extent = feature_pbf.varint();
        } else {
            feature_pbf.skip();
        }
    }
}

FilteredVectorTileLayer::FilteredVectorTileLayer(const VectorTileLayer& layer_, const FilterExpression& filterExpression_)
    : GeometryFilteredTileLayer(layer_, filterExpression_) {
      feature_pbf = layer_.feature_pbf;
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

    auto field_it = layer.key_index.find(key);
    if (field_it != layer.key_index.end()) {
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
                if (layer.values.size() > tag_val) {
                    value = layer.values[tag_val];
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

GeometryTileFeature FilteredVectorTileLayer::nextMatchingFeature() {
    GeometryTileFeature result;

    if (feature_pbf.next(2)) {
        pbf current_feature_pbf = feature_pbf.message();

        GeometryTileTagExtractor<pbf> extractor(layer);

        while (current_feature_pbf.next()) {
            if (current_feature_pbf.tag == 2) { // tags
                extractor.setTags(current_feature_pbf);
            } else if (current_feature_pbf.tag == 3) { // geometry type
                extractor.setType(GeometryFeatureType(current_feature_pbf.varint()));
            } else {
                current_feature_pbf.skip();
            }
        }

        if (evaluate(filterExpression, extractor)) {
            result = VectorTileFeature(current_feature_pbf.message(), layer);
        }
    }

    return std::move(result);
}

GeometryTileFeature VectorTileLayer::nextFeature() {
    GeometryTileFeature result;

    if (feature_pbf.next(2)) {
        result = VectorTileFeature(feature_pbf.message(), *this);
    }

    return std::move(result);
}
