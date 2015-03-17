#include <mbgl/map/live_tile_parser.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_source.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

// Note: see ~TileParser().
LiveTileParser::~LiveTileParser() = default;

LiveTileParser::LiveTileParser(const LiveTile* liveTile_,
                               LiveTileData& tile_,
                               const util::ptr<const Style>& style_,
                               GlyphAtlas& glyphAtlas_,
                               GlyphStore& glyphStore_,
                               SpriteAtlas& spriteAtlas_,
                               const util::ptr<Sprite>& sprite_)
    : TileParser::TileParser(nullptr, tile_, style_, glyphAtlas_, glyphStore_, spriteAtlas_, sprite_),
      liveTile(liveTile_) {}

std::unique_ptr<Bucket> LiveTileParser::createBucket(const StyleBucket& bucketDesc) {
    // Skip this bucket if we are to not render this
    if (tile.id.z < std::floor(bucketDesc.min_zoom) && std::floor(bucketDesc.min_zoom) < tile.source.max_zoom) return nullptr;
    if (tile.id.z >= std::ceil(bucketDesc.max_zoom)) return nullptr;
    if (bucketDesc.visibility == mbgl::VisibilityType::None) return nullptr;

    auto layer = liveTile->getLayer(bucketDesc.source_layer);
    if (layer) {
        if (bucketDesc.type == StyleLayerType::Fill) {
            return createFillBucket(*layer, bucketDesc);
        } else if (bucketDesc.type == StyleLayerType::Line) {
            return createLineBucket(*layer, bucketDesc);
        } else if (bucketDesc.type == StyleLayerType::Symbol) {
            return createSymbolBucket(*layer, bucketDesc);
        } else if (bucketDesc.type == StyleLayerType::Raster) {
            return nullptr;
        } else {
            fprintf(stderr, "[WARNING] unknown bucket render type for layer '%s' (source layer '%s')\n", bucketDesc.name.c_str(), bucketDesc.source_layer.c_str());
        }
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
        if (debug::tileParseWarnings) {
            fprintf(stderr, "[WARNING] layer '%s' does not exist in tile %d/%d/%d\n",
                    bucketDesc.source_layer.c_str(), tile.id.z, tile.id.x, tile.id.y);
        }
    }

    return nullptr;
}

};
