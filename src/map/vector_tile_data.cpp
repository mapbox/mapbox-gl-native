#include <llmr/map/vector_tile_data.hpp>

#include <llmr/map/map.hpp>
#include <llmr/map/tile_parser.hpp>

using namespace llmr;

VectorTileData::VectorTileData(Tile::ID id, Map &map, const std::string url)
    : TileData(id, map, url) {
}

VectorTileData::~VectorTileData() {
    map.getGlyphAtlas().removeGlyphs(id.to_uint64());
}

void VectorTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    try {
        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        TileParser parser(data, *this, map.getStyle(), map.getGlyphAtlas());
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return;
    }

    if (state != State::obsolete) {
        state = State::parsed;
    }
}

void VectorTileData::render(Painter &painter, const LayerDescription& layer_desc) {
    auto databucket_it = buckets.find(layer_desc.bucket_name);
    if (databucket_it != buckets.end()) {
        assert(databucket_it->second);
        databucket_it->second->render(painter, layer_desc.name, id);
    }
}