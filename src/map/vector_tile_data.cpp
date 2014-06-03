#include <llmr/map/vector_tile_data.hpp>
#include <llmr/util/std.hpp>
#include <llmr/map/map.hpp>

using namespace llmr;

VectorTileData::VectorTileData(Tile::ID id, Map &map, const std::string url)
    : TileData(id, map, url) {
}

VectorTileData::~VectorTileData() {
    map.getGlyphAtlas()->removeGlyphs(id.to_uint64());
}

void VectorTileData::beforeParse() {
    parser = std::make_unique<TileParser>(data, *this, map.getStyle(), map.getGlyphAtlas(), map.getGlyphStore(), map.getSpriteAtlas());
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

void VectorTileData::render(Painter &painter, const LayerDescription& layer_desc) {
    auto databucket_it = buckets.find(layer_desc.bucket_name);
    if (databucket_it != buckets.end()) {
        assert(databucket_it->second);
        databucket_it->second->render(painter, layer_desc.name, id);
    }
}

bool VectorTileData::hasData(const LayerDescription& layer_desc) const {
    if (state != State::parsed) return false;

    auto databucket_it = buckets.find(layer_desc.bucket_name);
    if (databucket_it != buckets.end()) {
        assert(databucket_it->second);
        return databucket_it->second->hasData();
    } else {
        return false;
    }
}
