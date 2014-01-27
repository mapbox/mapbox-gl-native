#include <llmr/map/tile.hpp>

#include <stdint.h>
#include <cassert>
// #include <iostream>
#include <thread>

#include <llmr/util/pbf.hpp>
#include <llmr/util/string.hpp>
#include <llmr/geometry/geometry.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/style/style.hpp>
#include <llmr/style/value.hpp>
#include <cmath>

using namespace llmr;

Tile::ID Tile::parent(const ID& id, int32_t z) {
    assert(z < id.z);
    ID pos(id);
    while (pos.z > z) {
        pos.z--;
        pos.x = floor(pos.x / 2);
        pos.y = floor(pos.y / 2);
    }
    return pos;
}


std::forward_list<Tile::ID> Tile::children(const ID& id, int32_t z) {
    assert(z > id.z);
    int32_t factor = pow(2, z - id.z);

    std::forward_list<ID> children;
    for (int32_t y = id.y * factor, y_max = (id.y + 1) * factor; y < y_max; y++) {
        for (int32_t x = id.x * factor, x_max = (id.x + 1) * factor; x < x_max; x++) {
            children.emplace_front(x, y, z);
        }
    }
    return children;
}


Tile::Tile(ID id, const Style& style)
    : id(id),
      state(initial),
      lineVertex(std::make_shared<linevertexbuffer>()),
      debugFontVertex(std::make_shared<debug_font_buffer>()),
      fillBuffer(std::make_shared<FillBuffer>()),
      data(0),
      bytes(0),
      style(style) {

    // Initialize tile debug coordinates
    char coord[32];
    snprintf(coord, sizeof(coord), "%d/%d/%d", id.z, id.x, id.y);
    debugFontVertex->addText(coord, 50, 200, 5);
}

Tile::~Tile() {
    std::lock_guard<std::mutex> lock(mtx);

    // fprintf(stderr, "[%p] deleting tile %d/%d/%d\n", this, id.z, id.x, id.y);
    if (this->data) {
        free(this->data);
        this->data = NULL;
    }
}

const std::string Tile::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}


void Tile::setData(uint8_t *data, uint32_t bytes) {
    this->data = (uint8_t *)malloc(bytes);
    this->bytes = bytes;
    memcpy(this->data, data, bytes);
}

void Tile::cancel() {
    // TODO: thread safety
    if (state != obsolete) {
        state = obsolete;
    } else {
        assert((!"logic error? multiple cancelleations"));
    }
}

bool Tile::parse() {
    std::lock_guard<std::mutex> lock(mtx);

    if (state == obsolete) {
        return false;
    }

    try {
        pbf tile(data, bytes);
        // fprintf(stderr, "[%p] parsing tile [%d/%d/%d]...\n", this, z, x, y);

        parseLayers(tile, style.layers);
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    }

    if (state == obsolete) {
        return false;
    } else {
        state = ready;
    }

    return true;
}

void Tile::parseLayers(const pbf& data, const std::vector<LayerDescription>& layers) {
    for (const LayerDescription& layer_desc : layers) {
        if (layer_desc.child_layer.size()) {
            // This is a layer group.
            // TODO: create framebuffer
            parseLayers(data, layer_desc.child_layer);
            // TODO: render framebuffer on previous framebuffer
        } else {
            // This is a singular layer. Check if this bucket already exists. If not,
            // parse this bucket.
            auto bucket_it = buckets.find(layer_desc.bucket_name);
            if (bucket_it == buckets.end()) {
                auto bucket_it = style.buckets.find(layer_desc.bucket_name);
                if (bucket_it != style.buckets.end()) {
                    // Only create the new bucket if we have an actual specification
                    // for it.
                    std::shared_ptr<Bucket> bucket = createBucket(data, bucket_it->second);
                    if (bucket) {
                        // Bucket creation might fail because the data tile may not
                        // contain any data that falls into this bucket.
                        buckets[layer_desc.bucket_name] = bucket;
                    }
                } else {
                    // There is no proper specification for this bucket, even though
                    // it is referenced in the stylesheet.
                    fprintf(stderr, "Stylesheet specifies bucket %s, but it is not defined\n", layer_desc.bucket_name.c_str());
                }
            }
        }
    }
}

std::shared_ptr<Bucket> Tile::createBucket(const pbf& data, const BucketDescription& bucket_desc) {
    pbf tile(data);
    // TODO: remember data locations in tiles for faster parsing so that we don't
    // have to go through the entire vector tile all the time.
    while (tile.next()) {
        if (tile.tag == 3) { // layer
            pbf layer = tile.message();
            while (layer.next()) {
                if (layer.tag == 1) {
                    std::string name = layer.string();
                    if (name == bucket_desc.source_layer) {
                        if (bucket_desc.type == BucketType::Fill) {
                            return createFillBucket(layer, bucket_desc);
                        } else {
                            // TODO: create other bucket types.
                        }
                    }
                } else {
                    layer.skip();
                }
            }
        } else {
            tile.skip();
        }
    }

    return NULL;
}

enum geom_type {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

std::shared_ptr<Bucket> Tile::createFillBucket(const pbf data, const BucketDescription& bucket_desc) {
    pbf layer;
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(fillBuffer);

    int32_t key = -1;
    std::set<uint32_t> values;

    // If we filter further by field/value, parse the key/value indices first
    // because protobuf doesn't mandate a particular key order.
    if (bucket_desc.source_field.size()) {
        uint32_t key_id = 0;
        uint32_t value_id = 0;

        // Find out what key/value IDs we need.
        layer = data;
        while (layer.next()) {
            if (layer.tag == 3) { // keys
                if (layer.string() == bucket_desc.source_field) {
                    // We found the key
                    key = key_id;
                }
                key_id++;
            } else if (layer.tag == 4) { // values
                Value value = parseValue(layer.message());
                if (std::find(bucket_desc.source_value.begin(), bucket_desc.source_value.end(), value) != bucket_desc.source_value.end()) {
                    values.insert(value_id);
                }
                value_id++;
            } else {
                layer.skip();
            }
        }

        if (key < 0 || values.empty()) {
            // There are no valid values that we could possibly find. Abort early.
            return bucket;
        }
    }

    // Now parse the features and optionally filter by key/value IDs.
    layer = data;
    while (layer.next()) {
        if (layer.tag == 2) { // feature
            pbf feature = layer.message();
            pbf geometry;
            geom_type type = Unknown;
            bool skip = false;

            while (!skip && feature.next()) {
                if (feature.tag == 2) { // tags
                    if (key < 0) {
                        // We want to parse the entire layer anyway
                        feature.skip();
                    } else {
                        // We only want to parse some features.
                        skip = true;
                        // tags are packed varints. They should have an even length.
                        pbf tags = feature.message();
                        while (tags) {
                            uint32_t tag_key = tags.varint();
                            if (tags) {
                                uint32_t tag_val = tags.varint();
                                // Now check if the tag_key/tag_val pair is included
                                // in the set of key/values that we are looking for.
                                if (key == tag_key && values.find(tag_val) != values.end()) {
                                    skip = false;
                                    break;
                                }
                            } else {
                                // This should not happen; otherwise the vector tile
                                // is invalid.
                                throw std::runtime_error("uneven number of feature tag ids");
                            }
                        }
                    }
                } else if (feature.tag == 3) { // geometry type
                    type = (geom_type)feature.varint();
                    if (type != Polygon) {
                        skip = true;
                        break;
                    }
                } else if (feature.tag == 4) { // geometry
                    geometry = feature.message();
                } else {
                    feature.skip();
                }
            }

            if (!skip && geometry) {
                bucket->addGeometry(geometry);
            }
        } else {
            layer.skip();
        }
    }

    return bucket;
}

// void Tile::addLineGeometry(pbf& geom) {
//     Geometry geometry(geom);

//     Geometry::command cmd;
//     int32_t x, y;
//     while ((cmd = geometry.next(x, y)) != Geometry::end) {
//         if (cmd == Geometry::move_to) {
//             lineVertex.addDegenerate();
//         }
//         lineVertex.addCoordinate(x, y);
//     }
// }
