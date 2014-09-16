#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/uv_detail.hpp>

using namespace mbgl;

TileData::TileData(Tile::ID id, Map &map, const SourceInfo &source)
    : id(id),
      state(State::initial),
      map(map),
      source(source),
      debugBucket(debugFontBuffer) {
    // Initialize tile debug coordinates
    const std::string str = util::sprintf<32>("%d/%d/%d", id.z, id.x, id.y);
    debugFontBuffer.addText(str.c_str(), 50, 200, 5);
}

TileData::~TileData() {
    cancel();
}

const std::string TileData::toString() const {
    return util::sprintf<32>("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void TileData::request() {
    if (source.tiles.empty())
        return;

    std::string url = source.tiles[(id.x + id.y) % source.tiles.size()];
    url = util::replaceTokens(url, [&](const std::string &token) -> std::string {
        if (token == "z") return std::to_string(id.z);
        if (token == "x") return std::to_string(id.x);
        if (token == "y") return std::to_string(id.y);
        if (token == "ratio") return (map.getState().getPixelRatio() > 1.0 ? "@2x" : "");
        return "";
    });

    state = State::loading;

    // Note: Somehow this feels slower than the change to request_http()
    std::weak_ptr<TileData> weak_tile = shared_from_this();
    req = map.getFileSource()->request(ResourceType::Tile, url);
    req->onload([weak_tile, url](const Response &res) {
        util::ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
            return;
        }

        // Clear the request object.
        tile->req.reset();

        if (res.code == 200) {
            tile->state = State::loaded;

            tile->data = res.data;

            // Schedule tile parsing in another thread
            tile->reparse();
        } else {
#if defined(DEBUG)
            fprintf(stderr, "[%s] tile loading failed: %ld, %s\n", url.c_str(), res.code, res.message.c_str());
#endif
        }
    });
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        req.reset();
    }
}

void TileData::beforeParse() {}

void TileData::reparse() {
    beforeParse();

    // We're creating a new work request. The work request deletes itself after it executed
    // the after work handler
    new uv::work<util::ptr<TileData>>(
        map.getLoop(),
        [](util::ptr<TileData> &tile) {
            tile->parse();
        },
        [](util::ptr<TileData> &tile) {
            tile->afterParse();
            tile->map.update();
        },
        shared_from_this());
}

void TileData::afterParse() {}
