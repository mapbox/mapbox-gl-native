#include <llmr/map/tile.hpp>

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

Tile::ID Tile::parent(const ID& id, int32_t z) {
    assert(z < id.z);
    ID pos(id);
    while (pos.z > z) {
        --pos.z;
        pos.x = floor(pos.x / 2);
        pos.y = floor(pos.y / 2);
    }
    return pos;
}


std::forward_list<Tile::ID> Tile::children(const ID& id, int32_t z) {
    assert(z > id.z);
    int32_t factor = pow(2, z - id.z);

    std::forward_list<ID> children;
    for (int32_t y = id.y * factor, y_max = (id.y + 1) * factor; y < y_max; ++y) {
        for (int32_t x = id.x * factor, x_max = (id.x + 1) * factor; x < x_max; ++x) {
            children.emplace_front(x, y, z);
        }
    }
    return children;
}


Tile::Tile(ID id, const Style& style, bool use_raster, bool use_retina)
    : id(id),
      state(initial),
      use_raster(use_raster),
      use_retina(use_retina),
      raster(),
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

Tile::~Tile() {
}

const std::string Tile::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void Tile::request() {
    state = Tile::loading;

    std::string url;

    if (use_raster) {
        url = util::sprintf(kTileRasterURL, id.z, id.x, id.y, (use_retina ? "@2x" : ""));
    } else {
        url = util::sprintf(kTileVectorURL, id.z, id.x, id.y);
    }

    std::shared_ptr<Tile> tile = shared_from_this();
    platform::Request *request = platform::request_http(url, [=](platform::Response& res) {
        if (res.code == 200 && tile->state != obsolete) {
            tile->state = Tile::loaded;
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

void Tile::cancel() {
    // TODO: thread safety
    if (state != obsolete) {
        state = obsolete;
        platform::cancel_request_http(req);
    } else {
        assert((!"logic error? multiple cancelleations"));
    }
}

bool Tile::parse() {
    // std::lock_guard<std::mutex> lock(mtx);

    if (state != loaded) {
        return false;
    }

    if (use_raster) {
        raster = std::make_shared<util::Raster>();
        raster->load(data);
        state = parsed;
        return true;
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
        state = parsed;
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

std::shared_ptr<Bucket> Tile::createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(fillVertexBuffer, triangleElementsBuffer, lineElementsBuffer, bucket_desc);

    // auto init_start = std::chrono::high_resolution_clock::now();

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
                bucket->tessellate();
            }
        }
    }

    // auto init_end = std::chrono::high_resolution_clock::now();
    // auto init = (double)std::chrono::duration_cast<std::chrono::microseconds>(init_end - init_start).count() / 1000.0;

    // auto tess_start = std::chrono::high_resolution_clock::now();
    // auto tess_end = std::chrono::high_resolution_clock::now();
    // auto tess = (double)std::chrono::duration_cast<std::chrono::microseconds>(tess_end - tess_start).count() / 1000.0;


    // fprintf(stderr, "%s/%s   init: %f   tess: %f\n", bucket_desc.source_layer.c_str(), bucket_desc.source_field.c_str(), init, tess);

    return bucket;
}

std::shared_ptr<Bucket> Tile::createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<LineBucket> bucket = std::make_shared<LineBucket>(lineVertexBuffer, triangleElementsBuffer, pointElementsBuffer, bucket_desc);

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

std::shared_ptr<Bucket> Tile::createPointBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<PointBucket> bucket = std::make_shared<PointBucket>(pointVertexBuffer, bucket_desc);

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
