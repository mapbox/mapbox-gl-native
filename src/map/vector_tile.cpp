#include <llmr/map/vector_tile.hpp>
#include <llmr/style/bucket_description.hpp>

#include <algorithm>
#include <iostream>

using namespace llmr;


std::ostream& llmr::operator<<(std::ostream& os, const FeatureType& type) {
    switch (type) {
        case FeatureType::Unknown: return os << "Unknown";
        case FeatureType::Point: return os << "Point";
        case FeatureType::LineString: return os << "LineString";
        case FeatureType::Polygon: return os << "Polygon";
        default: return os << "Invalid";
    }
}

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
            type = (FeatureType)feature.varint();
        } else if (feature.tag == 4) { // geometry
            geometry = feature.message();
        } else {
            feature.skip();
        }
    }
}


std::ostream& llmr::operator<<(std::ostream& os, const VectorTileFeature& feature) {
    os << "Feature(" << feature.id << "): " << feature.type << std::endl;
    for (const auto& prop : feature.properties) {
        os << "  - " << prop.first << ": " << prop.second << std::endl;
    }
    return os;
}


VectorTile::VectorTile() {}


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

VectorTile& VectorTile::operator=(VectorTile && other) {
    if (this != &other) {
        layers.swap(other.layers);
    }
    return *this;
}

VectorTileLayer::VectorTileLayer(pbf layer) : data(layer) {
    std::vector<std::string> stacks;

    while (layer.next()) {
        if (layer.tag == 1) { // name
            name = layer.string();
        } else if (layer.tag == 3) { // keys
            keys.emplace_back(layer.string());
        } else if (layer.tag == 4) { // values
            values.emplace_back(std::move(parseValue(layer.message())));
        } else if (layer.tag == 5) { // extent
            extent = layer.varint();
        } else {
            layer.skip();
        }
    }
}

FilteredVectorTileLayer::FilteredVectorTileLayer(const VectorTileLayer& layer, const BucketDescription& bucket_desc)
    : layer(layer),
      bucket_desc(bucket_desc) {
    // If we filter further by field/value, parse the key/value indices first
    // because protobuf doesn't mandate a particular key order.
    if (bucket_desc.source_field.size()) {
        // Get the ID of the desired layer key.
        auto key_it = std::find(layer.keys.begin(), layer.keys.end(), bucket_desc.source_field);
        if (key_it != layer.keys.end()) {
            key = (int32_t)(key_it - layer.keys.begin());
        }
        // Get the ID of the desired layer value(s), comparing as strings in
        // order to avoid numeric type mismatches between the layer and the
        // style definition.
        for (const Value& source_value : bucket_desc.source_value) {
            auto value_it = std::find_if(layer.values.begin(), layer.values.end(), util::relaxed_equal(source_value));
            if (value_it != layer.values.end()) {
                values.insert((uint32_t)(value_it - layer.values.begin()));
            }
        }
    }
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::begin() const {
    if (bucket_desc.source_field.size() && (key < 0 || values.empty())) {
        // There are no valid values that we could possibly find. Abort early.
        return end();
    } else {
        return iterator(*this, layer.data);
    }
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::end() const {
    return iterator(*this, pbf(layer.data.end, 0));
}

FilteredVectorTileLayer::iterator::iterator(const FilteredVectorTileLayer& filter, const pbf& data)
    : filter(filter),
      feature(pbf()),
      data(data) {
    operator++();
}

void FilteredVectorTileLayer::iterator::operator++() {
    valid = false;

    while (data.next(2)) { // feature
        feature = data.message();
        pbf feature_pbf = feature;
        BucketType type = BucketType::None;

        bool skip = false;
        while (!skip && feature_pbf.next()) {
            if (feature_pbf.tag == 2) { // tags
                if (filter.key < 0) {
                    // We want to parse the entire layer anyway
                    feature_pbf.skip();
                } else {
                    // We only want to parse some features.
                    skip = true;

                    // tags are packed varints. They should have an even length.
                    pbf tags_pbf = feature_pbf.message();
                    while (tags_pbf) {
                        int32_t tag_key = tags_pbf.varint();
                        if (tags_pbf) {
                            uint32_t tag_val = tags_pbf.varint();
                            // Now check if the tag_key/tag_val pair is included
                            // in the set of key/values that we are looking for.
                            if (filter.key == tag_key && filter.values.find(tag_val) != filter.values.end()) {
                                skip = false;
                                break; // feature_pbf loop
                            }
                        } else {
                            // This should not happen; otherwise the vector tile
                            // is invalid.
                            throw std::runtime_error("uneven number of feature tag ids");
                        }
                    }
                }
            } else if (feature_pbf.tag == 3) { // geometry type
                switch (feature_pbf.varint()) {
                    case 1 /* Point */:      type = BucketType::Icon; break;
                    case 2 /* LineString */: type = BucketType::Line; break;
                    case 3 /* Polygon */:    type = BucketType::Fill; break;
                    default:                 type = BucketType::None; break;
                }

                if (type != filter.bucket_desc.feature_type) {
                    break; // feature_pbf loop early
                }
            } else {
                feature_pbf.skip();
            }
        }

        if (!skip && type == filter.bucket_desc.feature_type) {
            valid = true;
            return; // data loop
        } else {
            valid = false;
        }
    }
}

bool FilteredVectorTileLayer::iterator::operator!=(const iterator& other) const {
    return !(data.data == other.data.data && data.end == other.data.end && valid == other.valid);
}

const pbf& FilteredVectorTileLayer::iterator:: operator*() const {
    return feature;
}
