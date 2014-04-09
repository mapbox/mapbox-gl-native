#include <llmr/map/tile_data.hpp>
#include <llmr/map/tile_parser.hpp>

#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/util/string.hpp>

#include <llmr/util/loop.hpp>

#include <uv.h>

using namespace llmr;

TileData::TileData(Tile::ID id, uv::loop &loop, const Style& style, GlyphAtlas& glyphAtlas, const bool use_raster, const bool use_retina)
    : id(id),
      use_raster(use_raster),
      use_retina(use_retina),
      state(State::initial),
      raster(),
      loop(loop),
      style(style),
      glyphAtlas(glyphAtlas) {

    // Initialize tile debug coordinates
    char coord[32];
    snprintf(coord, sizeof(coord), "%d/%d/%d", id.z, id.x, id.y);
    debugFontBuffer.addText(coord, 50, 200, 5);
}

TileData::~TileData() {
    glyphAtlas.removeGlyphs((uint64_t)id);
    cancel();
}

const std::string TileData::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void TileData::request(std::function<void()> callback) {
    state = State::loading;

    std::string url;

    if (use_raster) {
        url = util::sprintf(kTileRasterURL, id.z, id.x, id.y, (use_retina ? "@2x" : ""));
    } else {
        url = util::sprintf(kTileVectorURL, id.z, id.x, id.y);
    }

    std::weak_ptr<TileData> weak_tile = shared_from_this();
    req = platform::request_http(url, [weak_tile, callback](platform::Response *res) {
        // This callback function can be called in any thread.

        std::shared_ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
        } else if (res->code == 200) {
            tile->state = State::loaded;
            tile->data.swap(res->body);

            tile->reparse(callback);
        } else {
            fprintf(stderr, "tile loading failed\n");
        }
    });
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        platform::cancel_request_http(req.lock());
    }
}

void TileData::reparse(std::function<void()> callback) {
    // We are keeping a weak_ptr on this object. This means that it could be
    // destroyed at any point between now and when the work callback is called.
    std::weak_ptr<TileData> weak_tile = shared_from_this();
    loop.work([weak_tile]() {
        // This callback function will be called in the map render loop thread.

        // Once we're here, we try to retain the object and only continue with
        // parsing if the object is still alive and well.
        std::shared_ptr<TileData> tile = weak_tile.lock();
        if (tile && tile->state != State::obsolete) {
            tile->parse();
        }
    }, callback);
}

void TileData::parse() {
    if (state != State::loaded) {
        return;
    }

    if (use_raster) {
        raster = std::make_shared<Raster>();
        raster->load(data);
        state = State::parsed;
        return;
    }

    try {
        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        TileParser parser(data, *this, style, glyphAtlas);
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return;
    }

    if (state == State::obsolete) {
        return;
    } else {
        state = State::parsed;
    }
}
