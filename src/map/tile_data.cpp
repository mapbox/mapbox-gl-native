#include <llmr/map/tile_data.hpp>

#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/renderer/text_bucket.hpp>
#include <llmr/util/pbf.hpp>
#include <llmr/util/string.hpp>

#include <chrono>
#include <iostream>

using namespace llmr;

TileData::TileData(Tile::ID id, const Style& style)
    : id(id),
      state(State::initial),
      fillVertexBuffer(std::make_shared<FillVertexBuffer>()),
      lineVertexBuffer(std::make_shared<LineVertexBuffer>()),
      pointVertexBuffer(std::make_shared<PointVertexBuffer>()),
      textVertexBuffer(std::make_shared<TextVertexBuffer>()),
      triangleElementsBuffer(std::make_shared<TriangleElementsBuffer>()),
      lineElementsBuffer(std::make_shared<LineElementsBuffer>()),
      pointElementsBuffer(std::make_shared<PointElementsBuffer>()),
      style(style) {

    // Initialize tile debug coordinates
    char coord[32];
    snprintf(coord, sizeof(coord), "%d/%d/%d", id.z, id.x, id.y);
    debugFontBuffer.addText(coord, 50, 200, 5);
}

TileData::~TileData() {
    cancel();
}

const std::string TileData::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void TileData::request() {
    state = State::loading;

    // Create http request
    std::string url = util::sprintf(kTileURL,
        id.z, id.x, id.y);

    // Note: Somehow this feels slower than the change to request_http()
    std::weak_ptr<TileData> weak_tile = shared_from_this();
    platform::Request *request = platform::request_http(url, [=](platform::Response& res) {
        std::shared_ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
        } else if (res.code == 200) {
            tile->state = State::loaded;
            tile->data.swap(res.body);
            tile->parse();
        } else {
            fprintf(stderr, "tile loading failed\n");
        }
    }, []() {
        platform::restart();
    });
    req = request;
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        platform::cancel_request_http(req);
    }
}

bool TileData::parse() {
    if (state != State::loaded) {
        return false;
    }

    try {
        double parse_time_start = platform::time();
        VectorTile tile(pbf((const uint8_t *)data.data(), data.size()));
        parseStyleLayers(tile, style.layers);
        // double parse_time = (platform::time() - parse_time_start) * 1000.0;
        if (state == State::obsolete) {
            // fprintf(stderr, "[%p] parsing [%d/%d/%d] cancelled after %8.3fms\n", this, id.z, id.x, id.y, parse_time);
        } else {
            // fprintf(stderr, "[%p] parsing [%d/%d/%d] took %8.3fms\n", this, id.z, id.x, id.y, parse_time);
        }
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    }

    if (state == State::obsolete) {
        return false;
    } else {
        state = State::parsed;
    }

    return true;
}

void TileData::parseStyleLayers(const VectorTile& tile, const std::vector<LayerDescription>& layers) {
    for (const LayerDescription& layer_desc : layers) {
        // Cancel early when parsing.
        if (state == State::obsolete) {
            return;
        }

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

std::shared_ptr<Bucket> TileData::createBucket(const VectorTile& tile, const BucketDescription& bucket_desc) {
    auto layer_it = tile.layers.find(bucket_desc.source_layer);
    if (layer_it != tile.layers.end()) {
        const VectorTileLayer& layer = layer_it->second;
        if (bucket_desc.type == BucketType::Fill) {
            return createFillBucket(tile, layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Line) {
            return createLineBucket(tile, layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Point) {
            return createPointBucket(tile, layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Text) {
            return createTextBucket(tile, layer, bucket_desc);
        } else {
            throw std::runtime_error("unknown bucket type");
        }
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
    }

    return nullptr;
}

template <class Bucket>
void TileData::addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        if (state == State::obsolete) return;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }
}

std::shared_ptr<Bucket> TileData::createFillBucket(const VectorTile& tile, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(fillVertexBuffer, triangleElementsBuffer, lineElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return state == State::obsolete ? nullptr : bucket;
}

std::shared_ptr<Bucket> TileData::createLineBucket(const VectorTile& tile, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<LineBucket> bucket = std::make_shared<LineBucket>(lineVertexBuffer, triangleElementsBuffer, pointElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return state == State::obsolete ? nullptr : bucket;
}

std::shared_ptr<Bucket> TileData::createPointBucket(const VectorTile& tile, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<PointBucket> bucket = std::make_shared<PointBucket>(pointVertexBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return state == State::obsolete ? nullptr : bucket;
}

std::shared_ptr<Bucket> TileData::createTextBucket(const VectorTile& tile, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    // Determine the correct text stack.
    if (!layer.shaping.size()) {
        return nullptr;
    }

    // TODO: currently hardcoded to use the first font stack.
    const std::map<Value, std::vector<VectorTilePlacement>>& shaping = layer.shaping.begin()->second;

    std::vector<const VectorTileFace *> faces;
    for (const std::string& face : layer.faces) {
        auto it = tile.faces.find(face);
        if (it == tile.faces.end()) {
            // This layer references an unknown face.
            return nullptr;
        }
        faces.push_back(&it->second);
    }

    std::shared_ptr<TextBucket> bucket = std::make_shared<TextBucket>(textVertexBuffer, triangleElementsBuffer, bucket_desc);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (const pbf& feature_pbf : filtered_layer) {
        if (state == State::obsolete) return nullptr;
        bucket->addFeature({ feature_pbf, layer }, faces, shaping);
    }

    return bucket;
}
