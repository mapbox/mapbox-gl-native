#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression_private.hpp>
#include <mbgl/util/variant_io.hpp>

#include <ostream>

using namespace mbgl;

VectorTileFeature::VectorTileFeature(pbf feature, const VectorTileLayer& layer) {
    while (feature.next()) {
        if (feature.tag == 1) { // id
            id = feature.varint<uint64_t>();
        } else if (feature.tag == 2) { // tags
            // tags are packed varints. They should have an even length.
            pbf tags = feature.message();
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
        } else if (feature.tag == 3) { // type
            type = (GeometryFeatureType)feature.varint();
        } else if (feature.tag == 4) { // geometry
            geometries = feature.message();
        } else {
            feature.skip();
        }
    }
}

std::ostream& mbgl::operator<<(std::ostream& os, const GeometryTileFeature<pbf>& feature) {
    os << "Feature(" << feature.id << "): " << feature.type << std::endl;
    for (const auto& prop : feature.properties) {
        os << "  - " << prop.first << ": " << prop.second << std::endl;
    }
    return os;
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

VectorTileLayer::VectorTileLayer(pbf layer) : layer_pbf(layer) {
    while (layer.next()) {
        if (layer.tag == 1) { // name
            name = layer.string();
        } else if (layer.tag == 3) { // keys
            keys.emplace_back(layer.string());
            key_index.emplace(keys.back(), keys.size() - 1);
        } else if (layer.tag == 4) { // values
            values.emplace_back(std::move(parseValue(layer.message())));
        } else if (layer.tag == 5) { // extent
            extent = layer.varint();
        } else {
            layer.skip();
        }
    }
}

FilteredVectorTileLayer::FilteredVectorTileLayer(const VectorTileLayer& layer_, const FilterExpression& filterExpression_)
    : GeometryFilteredTileLayer<pbf>(layer_, filterExpression_) {}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::begin() const {
    const VectorTileLayer *layer_ = static_cast<const VectorTileLayer *>(&layer);

    return FilteredVectorTileLayer::iterator(*this, layer_->layer_pbf);
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::end() const {
    const VectorTileLayer *layer_ = static_cast<const VectorTileLayer *>(&layer);

    return FilteredVectorTileLayer::iterator(*this, pbf(layer_->layer_pbf.end, 0));
}

FilteredVectorTileLayer::iterator::iterator(const FilteredVectorTileLayer& parent_, const pbf& data_pbf_)
    : GeometryFilteredTileLayer<pbf>::iterator(parent_),
      feature_pbf(pbf()),
      data_pbf(data_pbf_) {}

VectorTileTagExtractor::VectorTileTagExtractor(const VectorTileLayer& layer_)
    : GeometryTileTagExtractor(layer_) {}

void VectorTileTagExtractor::setTags(const pbf& pbf) {
    tags_pbf = pbf;
}

mapbox::util::optional<Value> VectorTileTagExtractor::getValue(const std::string& key) const {
    if (key == "$type") {
        return Value(uint64_t(type));
    }

    mapbox::util::optional<Value> value;

    auto field_it = layer.key_index.find(key);
    if (field_it != layer.key_index.end()) {
        const uint32_t filter_key = field_it->second;

        // Now loop through all the key/value pair tags.
        // tags are packed varints. They should have an even length.
        pbf tags_pbf_ = tags_pbf;
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

template bool mbgl::evaluate(const FilterExpression&, const VectorTileTagExtractor&);

void FilteredVectorTileLayer::iterator::operator++() {
    valid = false;

    const FilterExpression &expression = parent.filterExpression;

    while (data_pbf.next(2)) { // feature
        feature_pbf = data_pbf.message();
        pbf feature_pbf_ = feature_pbf;

        GeometryTileTagExtractor<pbf> extractor(parent.layer);

        // Retrieve the basic information
        while (feature_pbf_.next()) {
            if (feature_pbf_.tag == 2) { // tags
                extractor.setTags(feature_pbf.message());
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
    return !(data_pbf.data == other.data_pbf.data && data_pbf.end == other.data_pbf.end && valid == other.valid);
}

const pbf& FilteredVectorTileLayer::iterator:: operator*() const {
    return feature_pbf;
}
