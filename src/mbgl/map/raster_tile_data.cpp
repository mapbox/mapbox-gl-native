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
                               TexturePool &texturePool,
                               const SourceInfo &source_,
                               Worker& worker_)
    : TileData(id_),
      source(source_),
      worker(worker_),
      bucket(texturePool, layout) {
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

        state = State::loaded;

        workRequest = worker.parseRasterTile(bucket, res.data, [this, callback] (TileParseResult result) {
            if (state != State::loaded) {
                return;
            }

            if (result.is<State>()) {
                state = result.get<State>();
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
    return &bucket;
}

void RasterTileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    if (req) {
        util::ThreadContext::getFileSource()->cancel(req);
        req = nullptr;
    }
    workRequest.reset();
}
