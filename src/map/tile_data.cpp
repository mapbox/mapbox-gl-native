#include <llmr/map/tile_data.hpp>
#include <llmr/map/map.hpp>

#include <llmr/util/string.hpp>

using namespace llmr;

TileData::TileData(Tile::ID id, Map &map, const std::string url)
    : id(id),
      state(State::initial),
      map(map),
      url(url),
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
    state = State::loading;

    // Note: Somehow this feels slower than the change to request_http()
    std::weak_ptr<TileData> weak_tile = shared_from_this();
    req = platform::request_http(url, [weak_tile](platform::Response *res) {
        std::shared_ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
        } else if (res->code == 200) {
            tile->state = State::loaded;

            tile->data.swap(res->body);

            // Schedule tile parsing in another thread
            tile->reparse();
        } else {
#if defined(DEBUG)
            fprintf(stderr, "[%s] tile loading failed: %d, %s\n", tile->url.c_str(), res->code, res->error_message.c_str());
#endif
        }
    }, map.getLoop());
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        platform::cancel_request_http(req.lock());
    }
}

void TileData::reparse() {
    // We're creating a new work request. The work request deletes itself after it executed
    // the after work handler
    new uv::work<std::shared_ptr<TileData>>(
        map.getLoop(),
        [](std::shared_ptr<TileData> &tile) { tile->parse(); },
        [](std::shared_ptr<TileData> &tile) { tile->map.update(); },
        shared_from_this());
}
