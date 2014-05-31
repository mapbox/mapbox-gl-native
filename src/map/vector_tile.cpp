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

FilteredVectorTileLayer::FilteredVectorTileLayer(const VectorTileLayer& layer, const BucketDescription& bucket_desc)
    : layer(layer),
      bucket_desc(bucket_desc) {
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::begin() const {
    return iterator(*this, layer.data);
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::end() const {
    return iterator(*this, pbf(layer.data.end, 0));
}

FilteredVectorTileLayer::iterator::iterator(const FilteredVectorTileLayer& parent, const pbf& data)
    : parent(parent),
      feature(pbf()),
      data(data) {
    operator++();
}

bool FilteredVectorTileLayer::iterator::matchesFilterExpression(const PropertyFilterExpression &filterExpression, const pbf &tags_pbf) {
    if (filterExpression.is<util::recursive_wrapper<PropertyFilter>>()) {
        return matchesFilter(filterExpression.get<util::recursive_wrapper<PropertyFilter>>().get(), tags_pbf);
    } else if (filterExpression.is<util::recursive_wrapper<PropertyExpression>>()) {
        return matchesExpression(filterExpression.get<util::recursive_wrapper<PropertyExpression>>().get(), tags_pbf);
    } else {
        return false;
    }
}


bool FilteredVectorTileLayer::iterator::matchesFilter(const PropertyFilter &filter, const pbf &const_tags_pbf) {
    auto field_it = parent.layer.key_index.find(filter.field);
    if (field_it != parent.layer.key_index.end()) {
        const uint32_t filter_key = field_it->second;

        // Now loop through all the key/value pair tags.
        // tags are packed varints. They should have an even length.
        pbf tags_pbf = const_tags_pbf;
        while (tags_pbf) {
            uint32_t tag_key = tags_pbf.varint();
            if (!tags_pbf) {
                // This should not happen; otherwise the vector tile
                // is invalid.
                throw std::runtime_error("uneven number of feature tag ids");
            }
            uint32_t tag_val = tags_pbf.varint();

            if (tag_key == filter_key) {
                if (parent.layer.values.size() > tag_val) {
                    const Value &value = parent.layer.values[tag_val];
                    return filter.compare(value);
                } else {
                    throw std::runtime_error("feature references out of range value");
                }
            }
        }
    }

    // The feature doesn't contain the field that we're looking to compare.
    // Depending on the filter, this may still be okay.
    return filter.isMissingFieldOkay();
}

bool FilteredVectorTileLayer::iterator::matchesExpression(const PropertyExpression &expression, const pbf &tags_pbf) {
    if (expression.op == ExpressionOperator::Or) {
        for (const PropertyFilterExpression &filterExpression : expression.operands) {
            if (matchesFilterExpression(filterExpression, tags_pbf)) {
                return true;
            }
        }
        return false;
    } else if (expression.op == ExpressionOperator::And) {
        for (const PropertyFilterExpression &filterExpression : expression.operands) {
            if (!matchesFilterExpression(filterExpression, tags_pbf)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}



void FilteredVectorTileLayer::iterator::operator++() {
    valid = false;

    const PropertyFilterExpression &expression = parent.bucket_desc.filter;

    while (data.next(2)) { // feature
        feature = data.message();
        pbf feature_pbf = feature;
        BucketType type = BucketType::None;

        bool matched = false;

        // Treat the absence of any expression filters as a match.
        if (expression.is<util::recursive_wrapper<PropertyExpression>>() && expression.get<util::recursive_wrapper<PropertyExpression>>().get().operands.size() == 0) {
            matched = true;
        }

        while (feature_pbf.next()) {
            if (feature_pbf.tag == 2) { // tags
                if (matched) {
                    // There is no filter, so we want to parse the entire layer anyway.
                    feature_pbf.skip();
                } else {
                    // We only want to parse some features.
                    const pbf tags_pbf = feature_pbf.message();
                    matched = matchesFilterExpression(expression, tags_pbf);
                    if (!matched) {
                        break; // feature_pbf loop early
                    }
                }
            } else if (feature_pbf.tag == 3) { // geometry type
                switch (feature_pbf.varint()) {
                    case 1 /* Point */:      type = BucketType::Icon; break;
                    case 2 /* LineString */: type = BucketType::Line; break;
                    case 3 /* Polygon */:    type = BucketType::Fill; break;
                    default:                 type = BucketType::None; break;
                }

                if (type != parent.bucket_desc.feature_type) {
                    matched = false;
                    break; // feature_pbf loop early
                }
            } else {
                feature_pbf.skip();
            }
        }

        if (matched) {
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
