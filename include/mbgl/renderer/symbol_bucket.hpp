#ifndef MBGL_RENDERER_SYMBOLBUCKET
#define MBGL_RENDERER_SYMBOLBUCKET

#include "bucket.hpp"
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/text/types.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <memory>
#include <map>
#include <vector>

namespace mbgl {

class Style;
class TextShader;
class IconShader;
class DotShader;
class Collision;
class SpriteAtlas;
class Sprite;
class GlyphAtlas;
class GlyphStore;
class FontStack;

class SymbolFeature {
public:
    pbf geometry;
    std::u32string label;
    std::string sprite;
};


class Symbol {
public:
    vec2<float> tl, tr, bl, br;
    Rect<uint16_t> tex;
    float angle;
    float minScale = 0.0f;
    float maxScale = std::numeric_limits<float>::infinity();
    CollisionAnchor anchor;
};

typedef std::vector<Symbol> Symbols;


class SymbolBucket : public Bucket {
    typedef ElementGroup<1> TextElementGroup;
    typedef ElementGroup<1> IconElementGroup;

public:
    SymbolBucket(const StyleBucketSymbol &properties, Collision &collision);

    virtual void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix);
    virtual bool hasData() const;
    virtual bool hasTextData() const;
    virtual bool hasIconData() const;

    void addFeatures(const VectorTileLayer &layer, const FilterExpression &filter,
                     const Tile::ID &id, SpriteAtlas &spriteAtlas, Sprite &sprite,
                     GlyphAtlas &glyphAtlas, GlyphStore &glyphStore);

    void addGlyphs(const PlacedGlyphs &glyphs, float placementZoom, PlacementRange placementRange,
                   float zoom);

    void drawGlyphs(TextShader &shader);
    void drawIcons(IconShader& shader);

private:

    std::vector<SymbolFeature> processFeatures(const VectorTileLayer &layer, const FilterExpression &filter, GlyphStore &glyphStore, const Sprite &sprite);


    void addFeature(const pbf &geom_pbf, const Shaping &shaping, const GlyphPositions &face, const Rect<uint16_t> &image);
    void addFeature(const std::vector<Coordinate> &line, const Shaping &shaping, const GlyphPositions &face, const Rect<uint16_t> &image);


    // Adds placed items to the buffer.
    template <typename Buffer>
    void addSymbols(Buffer &buffer, const PlacedGlyphs &symbols, float scale, PlacementRange placementRange);

    // Adds glyphs to the glyph atlas so that they have a left/top/width/height coordinates associated to them that we can use for writing to a buffer.
    void addGlyphsToAtlas(uint64_t tileid, const std::string stackname, const std::u32string &string,
                  const FontStack &fontStack, GlyphAtlas &glyphAtlas, GlyphPositions &face);

public:
    const StyleBucketSymbol &properties;

private:
    Collision &collision;

    struct {
        TextVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<TextElementGroup> groups;
    } text;

    struct {
        IconVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<IconElementGroup> groups;
    } icon;

};
}

#endif
