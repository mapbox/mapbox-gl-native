#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

TileData::TileData(Tile::ID const& id_, const SourceInfo& source_, Environment& env_)
    : id(id_),
      name(id),
      state(State::initial),
      source(source_),
      env(env_),
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
    if (source.tiles.empty())
        return;

    std::string url = source.tiles[(id.x + id.y) % source.tiles.size()];
    url = util::replaceTokens(url, [&](const std::string &token) -> std::string {
        if (token == "z") return util::toString(id.z);
        if (token == "x") return util::toString(id.x);
        if (token == "y") return util::toString(id.y);
        if (token == "prefix") {
            std::string prefix { 2 };
            prefix[0] = "0123456789abcdef"[id.x % 16];
            prefix[1] = "0123456789abcdef"[id.y % 16];
            return prefix;
        }
        if (token == "ratio") return pixelRatio > 1.0 ? "@2x" : "";
        return "";
    });

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
        [](util::ptr<TileData>& tile) {
            tile->parse();
        },
        [callback](util::ptr<TileData>&) {
            callback();
        },
        shared_from_this());
}
