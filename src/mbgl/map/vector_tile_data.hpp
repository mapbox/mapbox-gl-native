#ifndef MBGL_MAP_VECTOR_TILE_DATA
#define MBGL_MAP_VECTOR_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/fill_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>

#include <iosfwd>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace mbgl {

class Bucket;
class Painter;
class SourceInfo;
class StyleLayer;
class TileParser;
class GlyphAtlas;
class GlyphStore;
class SpriteAtlas;
class Sprite;
class Style;

class VectorTileData : public TileData {
    friend class TileParser;

public:
    VectorTileData(const TileID&,
                   float mapMaxZoom,
                   Style&,
                   GlyphAtlas&,
                   GlyphStore&,
                   SpriteAtlas&,
                   util::ptr<Sprite>,
                   const SourceInfo&);
    ~VectorTileData();

    void parse() override;
    virtual Bucket* getBucket(StyleLayer const &layer_desc) override;

    void setBucket(StyleLayer const &layer_desc, std::unique_ptr<Bucket> bucket);

protected:
    // Holds the actual geometries in this tile.
    FillVertexBuffer fillVertexBuffer;
    LineVertexBuffer lineVertexBuffer;

    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;

    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    SpriteAtlas& spriteAtlas;
    util::ptr<Sprite> sprite;
    Style& style;

private:
    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added to this std::map<> by the workers doing
    // the actual tile parsing as they get processed. Tiles partially
    // parsed can get new buckets at any moment but are also fit for
    // rendering. That said, access to this list needs locking unless
    // the tile is completely parsed.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    mutable std::mutex bucketsMutex;

public:
    const float depth;
};

}

#endif
