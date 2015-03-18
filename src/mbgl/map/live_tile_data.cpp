#include <mbgl/map/annotation.hpp>
#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/map/tile_parser.hpp>
#include <mbgl/style/style_source.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

LiveTileData::LiveTileData(Tile::ID const& id_,
                           AnnotationManager& annotationManager_,
                           float mapMaxZoom,
                           util::ptr<Style> style_,
                           GlyphAtlas& glyphAtlas_,
                           GlyphStore& glyphStore_,
                           SpriteAtlas& spriteAtlas_,
                           util::ptr<Sprite> sprite_,
                           const SourceInfo& source_)
    : VectorTileData::VectorTileData(id_, mapMaxZoom, style_, glyphAtlas_, glyphStore_,
                                     spriteAtlas_, sprite_, source_),
      annotationManager(annotationManager_) {
    // live features are always ready
    state = State::loaded;
}

LiveTileData::~LiveTileData() {}

void LiveTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    try {
        if (!style) {
            throw std::runtime_error("style isn't present in LiveTileData object anymore");
        }

        if (source.type == SourceType::Annotations) {
            const std::unique_ptr<LiveTile>& tile = annotationManager.getTile(id);

            if (tile) {
                // Parsing creates state that is encapsulated in TileParser. While parsing,
                // the TileParser object writes results into this objects. All other state
                // is going to be discarded afterwards.
                TileParser parser(*tile, *this, style, glyphAtlas, glyphStore, spriteAtlas, sprite);

                // Clear the style so that we don't have a cycle in the shared_ptr references.
                style.reset();

                parser.parse();
            } else {
                state = State::obsolete;
            }
        } else {
            throw std::runtime_error("unknown live tile source type");
        }
    } catch (const std::exception& ex) {
        Log::Error(Event::ParseTile, "Live-parsing [%d/%d/%d] failed: %s", id.z, id.x, id.y, ex.what());
        state = State::obsolete;
        return;
    }

    if (state != State::obsolete) {
        state = State::parsed;
    }
}
