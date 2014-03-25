#include <llmr/map/tile_data.hpp>
#include <llmr/map/tile_parser.hpp>

#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/util/string.hpp>

using namespace llmr;

TileData::TileData(Tile::ID id, const Style& style, GlyphAtlas& glyphAtlas, const bool use_raster, const bool use_retina)
    : id(id),
      use_raster(use_raster),
      use_retina(use_retina),
      state(State::initial),
      raster(),
      fillVertexBuffer(std::make_shared<FillVertexBuffer>()),
      lineVertexBuffer(std::make_shared<LineVertexBuffer>()),
      pointVertexBuffer(std::make_shared<PointVertexBuffer>()),
      textVertexBuffer(std::make_shared<TextVertexBuffer>()),
      triangleElementsBuffer(std::make_shared<TriangleElementsBuffer>()),
      lineElementsBuffer(std::make_shared<LineElementsBuffer>()),
      pointElementsBuffer(std::make_shared<PointElementsBuffer>()),
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

void TileData::request() {
    state = State::loading;

    std::string url;

    if (use_raster) {
        url = util::sprintf(kTileRasterURL, id.z, id.x, id.y, (use_retina ? "@2x" : ""));
    } else {
        url = util::sprintf(kTileVectorURL, id.z, id.x, id.y);
    }

    // Note: Somehow this feels slower than the change to request_http()
    std::weak_ptr<TileData> weak_tile = shared_from_this();
    platform::Request *request = platform::request_http(url, [=](platform::Response& res) {
        std::shared_ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
        } else if (res.code == 200) {
            tile->state = State::loaded;
            tile->data.swap(res.body);
            tile->parse();
        } else {
            fprintf(stderr, "tile loading failed\n");
        }
    }, []() {
        platform::restart();
    });
    req = request;
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        platform::cancel_request_http(req);
    }
}

bool TileData::parse() {
    if (state != State::loaded) {
        return false;
    }

    if (use_raster) {
        raster = std::make_shared<Raster>();
        raster->load(data);
        state = State::parsed;
        return true;
    }

    try {
        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        TileParser parser(data, *this, style, glyphAtlas);
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    }

    if (state == State::obsolete) {
        return false;
    } else {
        state = State::parsed;
    }

    return true;
}
