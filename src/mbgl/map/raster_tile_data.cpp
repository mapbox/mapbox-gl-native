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
        if (res.error) {
            std::exception_ptr error;
            if (res.error->reason == Response::Error::Reason::NotFound) {
                // This is a 404 response. We're treating these as empty tiles.
                workRequest.reset();
                state = State::parsed;
                bucket.reset();
            } else {
                // This is a different error, e.g. a connection or server error.
                error = std::make_exception_ptr(std::runtime_error(res.error->message));
            }
            callback(error);
            return;
        }

        modified = res.modified;
        expires = res.expires;

        if (res.notModified) {
            // We got the same data again. Abort early.
            return;
        }

        if (state == State::loading) {
            // Only overwrite the state when we didn't have a previous tile.
            state = State::loaded;
        }

        workRequest.reset();
        workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(texturePool), res.data, [this, callback] (RasterTileParseResult result) {
            workRequest.reset();
            if (state != State::loaded) {
                return;
            }

            std::exception_ptr error;
            if (result.is<std::unique_ptr<Bucket>>()) {
                state = State::parsed;
                bucket = std::move(result.get<std::unique_ptr<Bucket>>());
            } else {
                error = result.get<std::exception_ptr>();
                state = State::obsolete;
                bucket.reset();
            }

            callback(error);
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
