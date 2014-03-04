#include <llmr/map/tile_data.hpp>

#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/util/pbf.hpp>
#include <llmr/util/string.hpp>

#include <chrono>

using namespace llmr;

TileData::TileData(Tile::ID id, const Style& style)
    : id(id),
      state(initial),
      fillVertexBuffer(std::make_shared<FillVertexBuffer>()),
      lineVertexBuffer(std::make_shared<LineVertexBuffer>()),
      pointVertexBuffer(std::make_shared<PointVertexBuffer>()),
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
}

const std::string TileData::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void TileData::request() {
    state = loading;

    // Create http request
    std::string url = util::sprintf(kTileURL,
        id.z, id.x, id.y);

    // Note: Somehow this feels slower than the change to request_http()
    std::shared_ptr<TileData> tile = shared_from_this();
    platform::Request *request = platform::request_http(url, [=](platform::Response& res) {
        if (res.code == 200 && tile->state != obsolete) {
            tile->state = loaded;
            tile->data.swap(res.body);
            tile->parse();
        } else if (tile->state != obsolete) {
            fprintf(stderr, "tile loading failed\n");
        }
    }, []() {
        platform::restart();
    });
    req = request;
}

void TileData::cancel() {
    if (state != obsolete) {
        state = obsolete;
        platform::cancel_request_http(req);
    } else {
        assert((!"logic error? multiple cancelleations"));
    }
}

bool TileData::parse() {
    if (state != loaded) {
        return false;
    }

    try {
        double parse_time_start = platform::time();
        VectorTile tile(pbf((const uint8_t *)data.data(), data.size()));
        parseStyleLayers(tile, style.layers);
        double parse_time = (platform::time() - parse_time_start) * 1000.0;
        if (state == obsolete) {
            fprintf(stderr, "[%p] parsing [%d/%d/%d] cancelled after %8.3fms\n", this, id.z, id.x, id.y, parse_time);
        } else {
            fprintf(stderr, "[%p] parsing [%d/%d/%d] took %8.3fms\n", this, id.z, id.x, id.y, parse_time);
        }
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    }

    if (state == obsolete) {
        return false;
    } else {
        state = parsed;
    }

    return true;
}

void TileData::parseStyleLayers(const VectorTile& tile, const std::vector<LayerDescription>& layers) {
    for (const LayerDescription& layer_desc : layers) {
        // Cancel early when parsing.
        if (state == obsolete) {
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
            return createFillBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Line) {
            return createLineBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Point) {
            return createPointBucket(layer, bucket_desc);
        } else {
            // TODO: create other bucket types.
        }
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
    }

    return nullptr;
}

std::shared_ptr<Bucket> TileData::createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(fillVertexBuffer, triangleElementsBuffer, lineElementsBuffer, bucket_desc);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        if (state == obsolete) return nullptr;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
                bucket->tessellate();
            }
        }
    }

    return bucket;
}

std::shared_ptr<Bucket> TileData::createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<LineBucket> bucket = std::make_shared<LineBucket>(lineVertexBuffer, triangleElementsBuffer, pointElementsBuffer, bucket_desc);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        if (state == obsolete) return nullptr;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }

    return bucket;
}

std::shared_ptr<Bucket> TileData::createPointBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<PointBucket> bucket = std::make_shared<PointBucket>(pointVertexBuffer, bucket_desc);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        if (state == obsolete) return nullptr;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }

    return bucket;
}
