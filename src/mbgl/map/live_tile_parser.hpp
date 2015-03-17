#ifndef MBGL_MAP_LIVE_TILE_PARSER
#define MBGL_MAP_LIVE_TILE_PARSER

#include <mbgl/map/tile_parser.hpp>
#include <mbgl/map/live_tile.hpp>

namespace mbgl {

class LiveTileData;

class LiveTileParser : public TileParser {
public:
    LiveTileParser(const LiveTile* liveTile,
                   LiveTileData& tile,
                   const util::ptr<const Style>& style,
                   GlyphAtlas& glyphAtlas,
                   GlyphStore& glyphStore,
                   SpriteAtlas& spriteAtlas,
                   const util::ptr<Sprite>& sprite);
    ~LiveTileParser();

private:
    std::unique_ptr<Bucket> createBucket(const StyleBucket&) override;

private:
    const LiveTile* liveTile;
};

}

#endif
