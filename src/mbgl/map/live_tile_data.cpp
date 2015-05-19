#include <mbgl/map/annotation.hpp>
#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/map/tile_parser.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

LiveTileData::LiveTileData(const TileID& id_,
                           AnnotationManager& annotationManager_,
                           float mapMaxZoom,
                           Style& style_,
                           GlyphAtlas& glyphAtlas_,
                           GlyphStore& glyphStore_,
                           SpriteAtlas& spriteAtlas_,
                           util::ptr<Sprite> sprite_,
                           const SourceInfo& source_)
    : VectorTileData::VectorTileData(id_, mapMaxZoom, style_, glyphAtlas_, glyphStore_,
                                     spriteAtlas_, sprite_, source_),
      annotationManager(annotationManager_) {
    // live features are always ready
    setState(State::loaded);
}

LiveTileData::~LiveTileData() {
    // Cancel in most derived class destructor so that worker tasks are joined before
    // any member data goes away.
    cancel();
}

void LiveTileData::parse() {
    if (getState() != State::loaded) {
        return;
    }

    const LiveTile* tile = annotationManager.getTile(id);

    if (tile) {
        try {
            // Parsing creates state that is encapsulated in TileParser. While parsing,
            // the TileParser object writes results into this objects. All other state
            // is going to be discarded afterwards.
            TileParser parser(*tile, *this, style, glyphAtlas, glyphStore, spriteAtlas, sprite);
            parser.parse();
        } catch (const std::exception& ex) {
            Log::Error(Event::ParseTile, "Live-parsing [%d/%d/%d] failed: %s", id.z, id.x, id.y, ex.what());
            setState(State::obsolete);
            return;
        }
    }

    if (getState() != State::obsolete) {
        setState(State::parsed);
    }
}
