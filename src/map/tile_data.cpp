#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/filesource.hpp>
#include <mbgl/util/uv_detail.hpp>

using namespace mbgl;

TileData::TileData(Tile::ID id, Map &map, const SourceInfo &source)
    : id(id),
      state(State::initial),
      map(map),
      source(source),
      url(util::replaceTokens(source.url, [&](const std::string &token) -> std::string {
          if (token == "z") return std::to_string(id.z);
          if (token == "x") return std::to_string(id.x);
          if (token == "y") return std::to_string(id.y);
          if (token == "ratio") return (map.getState().getPixelRatio() > 1.0 ? "@2x" : "");
          return "";
      })),
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
    map.getFileSource()->load(ResourceType::Tile, url, [weak_tile](platform::Response *res) {
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
    });
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        platform::cancel_request_http(req.lock());
    }
}

void TileData::beforeParse() {}

void TileData::reparse() {
    beforeParse();

    // We're creating a new work request. The work request deletes itself after it executed
    // the after work handler
    new uv::work<std::shared_ptr<TileData>>(
        map.getLoop(),
        [](std::shared_ptr<TileData> &tile) {
            tile->parse();
        },
        [](std::shared_ptr<TileData> &tile) {
            tile->afterParse();
            tile->map.update();
        },
        shared_from_this());
}

void TileData::afterParse() {}
