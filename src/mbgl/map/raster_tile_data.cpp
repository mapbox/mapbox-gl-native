#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

#include <sstream>

using namespace mbgl;

RasterTileData::RasterTileData(const TileID& id_,
                               TexturePool &texturePool_,
                               const SourceInfo &source_,
                               Worker& worker_)
    : TileData(id_),
      texturePool(texturePool_),
      source(source_),
      worker(worker_) {
}

RasterTileData::~RasterTileData() {
    cancel();
}

void RasterTileData::request(float pixelRatio,
                             const std::function<void()>& callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Tile, url }, util::RunLoop::getLoop(), [url, callback, this](const Response &res) {
        if (res.stale) {
            // Only handle fresh responses.
            return;
        }
        req = nullptr;

        if (res.status == Response::NotFound) {
            state = State::parsed;
            callback();
            return;
        }

        if (res.status != Response::Successful) {
            std::stringstream message;
            message <<  "Failed to load [" << url << "]: " << res.message;
            error = message.str();
            state = State::obsolete;
            callback();
            return;
        }

        if (state == State::loading) {
            // Only overwrite the state when we didn't have a previous tile.
            state = State::loaded;
        }

        workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(texturePool, layout), res.data, [this, callback] (TileParseResult result) {
            workRequest.reset();
            if (state != State::loaded) {
                return;
            }

            if (result.is<TileParseResultBuckets>()) {
                auto& buckets = result.get<TileParseResultBuckets>();
                state = buckets.state;
                // TODO: Make this less awkward; we're only getting one bucket back.
                if (!buckets.buckets.empty()) {
                    bucket = std::move(buckets.buckets.front().second);
                }
            } else {
                std::stringstream message;
                message << "Failed to parse [" << std::string(id) << "]: " << result.get<std::string>();
                error = message.str();
                state = State::obsolete;
            }

            callback();
        });
    });
}

Bucket* RasterTileData::getBucket(StyleLayer const&) {
    return bucket.get();
}

void RasterTileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    req = nullptr;
    workRequest.reset();
}
