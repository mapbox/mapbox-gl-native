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
    void render(Painter &painter, const StyleLayer &layer_desc, const mat4 &matrix) override;
    bool hasData(StyleLayer const& layer_desc) const override;

protected:
    // Holds the actual geometries in this tile.
    FillVertexBuffer fillVertexBuffer;
    LineVertexBuffer lineVertexBuffer;

    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    SpriteAtlas& spriteAtlas;
    util::ptr<Sprite> sprite;
    Style& style;

public:
    const float depth;
};

}

#endif
