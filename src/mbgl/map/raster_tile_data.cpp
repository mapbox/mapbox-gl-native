#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

#include <sstream>

using namespace mbgl;

RasterTileData::RasterTileData(const TileID& id_, TexturePool &texturePool,
                               const SourceInfo &source_)
    : TileData(id_, source_), bucket(texturePool, layout) {
}

RasterTileData::~RasterTileData() {
    // Cancel in most derived class destructor so that worker tasks are joined before
    // any member data goes away.
    cancel();
}

void RasterTileData::request(Worker& worker,
                       float pixelRatio,
                       const std::function<void()>& callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Tile, url }, util::RunLoop::current.get()->get(), [url, callback, &worker, this](const Response &res) {
        req = nullptr;

        if (res.status != Response::Successful) {
            std::stringstream message;
            message <<  "Failed to load [" << url << "]: " << res.message;
            setError(message.str());
            callback();
            return;
        }

        state = State::loaded;
        data = res.data;

        // Schedule tile parsing in another thread
        reparse(worker, callback);
    });
}

bool RasterTileData::reparse(Worker& worker, std::function<void()> callback) {
    if (!mayStartParsing()) {
        return false;
    }

    workRequest = worker.send([this] { parse(); endParsing(); }, callback);
    return true;
}

void RasterTileData::parse() {
    if (getState() != State::loaded) {
        return;
    }

    if (bucket.setImage(data)) {
        setState(State::parsed);
    } else {
        setState(State::invalid);
    }
}

Bucket* RasterTileData::getBucket(StyleLayer const&) {
    return &bucket;
}
