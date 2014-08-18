#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/tile_parser.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>

using namespace mbgl;

VectorTileData::VectorTileData(Tile::ID id, Map &map, const SourceInfo &source)
    : TileData(id, map, source),
      depth(id.z >= source.max_zoom ? map.getMaxZoom() - id.z : 1) {
}

VectorTileData::~VectorTileData() {
    std::shared_ptr<GlyphAtlas> glyphAtlas = map.getGlyphAtlas();
    if (glyphAtlas) {
        glyphAtlas->removeGlyphs(id.to_uint64());
    }
}

void VectorTileData::beforeParse() {

    parser = std::make_unique<TileParser>(data, *this, map.getStyle(), map.getGlyphAtlas(), map.getGlyphStore(), map.getSpriteAtlas(), map.getSprite());
}

void VectorTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    try {
        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        parser->parse();
    } catch (const std::exception& ex) {
#if defined(DEBUG)
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
#endif
        cancel();
        return;
    }

    if (state != State::obsolete) {
        state = State::parsed;
    }
}

void VectorTileData::afterParse() {
    parser.reset();
}

void VectorTileData::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const mat4 &matrix) {
    if (state == State::parsed && layer_desc->bucket) {
        auto databucket_it = buckets.find(layer_desc->bucket->name);
        if (databucket_it != buckets.end()) {
            assert(databucket_it->second);
            databucket_it->second->render(painter, layer_desc, id, matrix);
        }
    }
}

bool VectorTileData::hasData(std::shared_ptr<StyleLayer> layer_desc) const {
    if (state == State::parsed && layer_desc->bucket) {
        auto databucket_it = buckets.find(layer_desc->bucket->name);
        if (databucket_it != buckets.end()) {
            assert(databucket_it->second);
            return databucket_it->second->hasData();
        }
    }
    return false;
}
