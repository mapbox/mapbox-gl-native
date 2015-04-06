#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/source.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

TileData::TileData(Tile::ID const& id_, const SourceInfo& source_)
    : id(id_),
      name(id),
      state(State::initial),
      source(source_),
      env(Environment::Get()),
      debugBucket(debugFontBuffer) {
    // Initialize tile debug coordinates
    debugFontBuffer.addText(name.c_str(), 50, 200, 5);
}

TileData::~TileData() {
    if (req) {
        env.cancelRequest(req);
    }
}

const std::string TileData::toString() const {
    return std::string { "[tile " } + name + "]";
}

void TileData::request(uv::worker &worker, float pixelRatio, std::function<void()> callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    std::weak_ptr<TileData> weak_tile = shared_from_this();
    req = env.request({ Resource::Kind::Tile, url }, [weak_tile, url, callback, &worker](const Response &res) {
        util::ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
            return;
        }

        // Clear the request object.
        tile->req = nullptr;

        if (res.status == Response::Successful) {
            tile->state = State::loaded;

            tile->data = res.data;

            // Schedule tile parsing in another thread
            tile->reparse(worker, callback);
        } else {
            Log::Error(Event::HttpRequest, "[%s] tile loading failed: %s", url.c_str(), res.message.c_str());
        }
    });
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    if (req) {
        env.cancelRequest(req);
        req = nullptr;
    }
}

void TileData::reparse(uv::worker& worker, std::function<void()> callback)
{
    // We're creating a new work request. The work request deletes itself after it executed
    // the after work handler
    new uv::work<util::ptr<TileData>>(
        worker,
        [this](util::ptr<TileData>& tile) {
            EnvironmentScope scope(env, ThreadType::TileWorker, "TileWorker_" + tile->name);
            tile->parse();
        },
        [callback](util::ptr<TileData>&) {
            callback();
        },
        shared_from_this());
}
