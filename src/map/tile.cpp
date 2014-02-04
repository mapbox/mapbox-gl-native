#include <llmr/map/tile.hpp>

#include <stdint.h>
#include <cassert>
// #include <iostream>
#include <thread>

#include <llmr/util/pbf.hpp>
#include <llmr/util/string.hpp>
#include <llmr/geometry/geometry.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/style/style.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/util/string.hpp>
#include <llmr/map/vector_tile.hpp>

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
      debugFontBuffer(std::make_shared<DebugFontBuffer>()),
      fillBuffer(std::make_shared<FillBuffer>()),
      lineBuffer(std::make_shared<LineBuffer>()),
      style(style) {

    // Initialize tile debug coordinates
    char coord[32];
    snprintf(coord, sizeof(coord), "%d/%d/%d", id.z, id.x, id.y);
    debugFontBuffer->addText(coord, 50, 200, 5);
}

Tile::~Tile() {
}

const std::string Tile::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void Tile::request() {
    state = Tile::loading;

    // Create http request
    std::string url = util::sprintf("http://localhost:3333/gl/tiles/plain/%d-%d-%d.vector.pbf",
        id.z, id.x, id.y);

    // Note: Somehow this feels slower than the change to request_http()
    std::shared_ptr<Tile> tile = shared_from_this();
    platform::request_http(url, [=](platform::Response& res) {
        if (res.code == 200) {
            tile->data.swap(res.body);
            tile->parse();
        } else {
            fprintf(stderr, "tile loading failed\n");
        }
    }, []() {
        // TODO: Make sure this gets passed the correct map ID/pointer.
        platform::restart(NULL);
    });
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
    // std::lock_guard<std::mutex> lock(mtx);

    if (state == obsolete) {
        return false;
    }

    try {
        VectorTile tile(pbf((const uint8_t *)data.data(), data.size()));
        parseStyleLayers(tile, style.layers);
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

void Tile::parseStyleLayers(const VectorTile& tile, const std::vector<LayerDescription>& layers) {
    for (const LayerDescription& layer_desc : layers) {
        if (layer_desc.child_layer.size()) {
            // This is a layer group.
            // TODO: create framebuffer
            parseStyleLayers(tile, layer_desc.child_layer);
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
                    std::shared_ptr<Bucket> bucket = createBucket(tile, bucket_it->second);
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

std::shared_ptr<Bucket> Tile::createBucket(const VectorTile& tile, const BucketDescription& bucket_desc) {
    auto layer_it = tile.layers.find(bucket_desc.source_layer);
    if (layer_it != tile.layers.end()) {
        const VectorTileLayer& layer = layer_it->second;
        if (bucket_desc.type == BucketType::Fill) {
            return createFillBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Line) {
            return createLineBucket(layer, bucket_desc);
        } else {
            // TODO: create other bucket types.
        }
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
    }

    return NULL;
}

std::shared_ptr<Bucket> Tile::createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(fillBuffer, bucket_desc);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }

    return bucket;
}

std::shared_ptr<Bucket> Tile::createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<LineBucket> bucket = std::make_shared<LineBucket>(lineBuffer, bucket_desc);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }

    return bucket;
}
