#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const TileID& id_,
                               TexturePool &texturePool_,
                               Worker& worker_)
    : TileData(id_),
      texturePool(texturePool_),
      worker(worker_) {
}

RasterTileData::~RasterTileData() {
    cancel();
}

void RasterTileData::request(const std::string& url,
                             const RasterTileData::Callback& callback) {
    state = State::loading;

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Tile, url }, [url, callback, this](Response res) {
        if (res.stale) {
            // Only handle fresh responses.
            return;
        }
        req = nullptr;

        if (res.error) {
            if (res.error->reason == Response::Error::Reason::NotFound) {
                state = State::parsed;
            } else {
                error = std::make_exception_ptr(std::runtime_error(res.error->message));
                state = State::obsolete;
            }
            callback();
            return;
        }

        if (state == State::loading) {
            // Only overwrite the state when we didn't have a previous tile.
            state = State::loaded;
        }

        modified = res.modified;
        expires = res.expires;

        workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(texturePool), res.data, [this, callback] (RasterTileParseResult result) {
            workRequest.reset();
            if (state != State::loaded) {
                return;
            }

            if (result.is<std::unique_ptr<Bucket>>()) {
                state = State::parsed;
                bucket = std::move(result.get<std::unique_ptr<Bucket>>());
            } else {
                error = result.get<std::exception_ptr>();
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
