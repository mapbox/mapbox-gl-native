#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/tile_parser.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

VectorTileData::VectorTileData(Tile::ID const& id_,
                               float mapMaxZoom, util::ptr<Style> style_,
                               GlyphAtlas& glyphAtlas_, GlyphStore& glyphStore_,
                               SpriteAtlas& spriteAtlas_, util::ptr<Sprite> sprite_,
                               const SourceInfo& source_, FileSource &fileSource_)
    : TileData(id_, source_, fileSource_),
      glyphAtlas(glyphAtlas_),
      glyphStore(glyphStore_),
      spriteAtlas(spriteAtlas_),
      sprite(sprite_),
      style(style_),
      depth(id.z >= source.max_zoom ? mapMaxZoom - id.z : 1) {
}

VectorTileData::~VectorTileData() {
    glyphAtlas.removeGlyphs(id.to_uint64());
}


void VectorTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    try {
        if (!style) {
            throw std::runtime_error("style isn't present in VectorTileData object anymore");
        }

        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        TileParser parser(data, *this, style,
                          glyphAtlas, glyphStore,
                          spriteAtlas, sprite);
        // Clear the style so that we don't have a cycle in the shared_ptr references.
        style.reset();

        parser.parse();
    } catch (const std::exception& ex) {
        Log::Error(Event::ParseTile, "Parsing [%d/%d/%d] failed: %s", id.z, id.x, id.y, ex.what());
        state = State::obsolete;
        return;
    }

    if (state != State::obsolete) {
        state = State::parsed;
    }
}

void VectorTileData::render(Painter &painter, util::ptr<StyleLayer> layer_desc, const mat4 &matrix) {
    if (state == State::parsed && layer_desc->bucket) {
        auto databucket_it = buckets.find(layer_desc->bucket->name);
        if (databucket_it != buckets.end()) {
            assert(databucket_it->second);
            databucket_it->second->render(painter, layer_desc, id, matrix);
        }
    }
}

bool VectorTileData::hasData(StyleLayer const& layer_desc) const {
    if (state == State::parsed && layer_desc.bucket) {
        auto databucket_it = buckets.find(layer_desc.bucket->name);
        if (databucket_it != buckets.end()) {
            assert(databucket_it->second);
            return databucket_it->second->hasData();
        }
    }
    return false;
}
