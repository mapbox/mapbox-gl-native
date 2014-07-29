#ifndef MBGL_RENDERER_SYMBOLBUCKET
#define MBGL_RENDERER_SYMBOLBUCKET

#include "bucket.hpp"
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/text/types.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <memory>
#include <map>
#include <vector>

namespace mbgl {

class Style;
class TextVertexBuffer;
class IconVertexBuffer;
class TriangleElementsBuffer;
class TextShader;
class IconShader;
class DotShader;
class Placement;
class SpriteAtlas;
class GlyphAtlas;
class GlyphStore;
class FontStack;

class SymbolBucket : public Bucket {
    typedef ElementGroup triangle_group_type;

public:
    SymbolBucket(
        TextVertexBuffer &textVertexBuffer,
        IconVertexBuffer& iconVertexBuffer,
        TriangleElementsBuffer &triangleElementsBuffer,
        const StyleBucketSymbol &properties, Placement &placement);

    virtual void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id);
    virtual bool hasData() const;
    virtual bool hasTextData() const;
    virtual bool hasIconData() const;

    void addFeatures(const VectorTileLayer &layer, const FilterExpression &filter,
                     const Tile::ID &id, SpriteAtlas &spriteAtlas, GlyphAtlas &glyphAtlas,
                     GlyphStore &glyphStore);

    void addGlyphs(const PlacedGlyphs &glyphs, float placementZoom, PlacementRange placementRange,
                   float zoom);


    void drawGlyphs(TextShader &shader);
    void drawIcons(IconShader& shader);
    void drawIcons(DotShader& shader);

private:
    void addGlyph(uint64_t tileid, const std::string stackname, const std::u32string &string,
                  const FontStack &fontStack, GlyphAtlas &glyphAtlas, GlyphPositions &face);
    void addFeature(const pbf &geom_pbf, const GlyphPositions &face, const Shaping &shaping);

public:
    const StyleBucketSymbol &properties;

private:
    TextVertexBuffer& textVertexBuffer;
    IconVertexBuffer& iconVertexBuffer;
    TriangleElementsBuffer& triangleElementsBuffer;
    Placement &placement;

    const size_t text_vertex_start;
    const size_t icon_vertex_start;
    const size_t triangle_elements_start;
    size_t icon_vertex_end = 0;
    VertexArrayObject array;

    std::vector<triangle_group_type> triangleGroups;
};
}

#endif
