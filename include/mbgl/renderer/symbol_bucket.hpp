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
class Placement;
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

class SymbolBucket : public Bucket {
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
                     const Tile::ID &id, SpriteAtlas &spriteAtlas, Sprite &sprite,
                     GlyphAtlas &glyphAtlas, GlyphStore &glyphStore);

    void addGlyphs(const PlacedGlyphs &glyphs, float placementZoom, PlacementRange placementRange,
                   float zoom);


    void drawGlyphs(TextShader &shader);
    void drawIcons(IconShader& shader);
    void drawIcons(DotShader& shader);

private:

    std::vector<SymbolFeature> processFeatures(const VectorTileLayer &layer, const FilterExpression &filter, GlyphStore &glyphStore, const Sprite &sprite);
    void addFeature(const pbf &geom_pbf, const Shaping &shaping, const Rect<uint16_t> &image);
    void addFeature(const std::vector<Coordinate> &line, const Shaping &shaping, const Rect<uint16_t> &image);




    void addGlyph(uint64_t tileid, const std::string stackname, const std::u32string &string,
                  const FontStack &fontStack, GlyphAtlas &glyphAtlas, GlyphPositions &face);
    void addFeature(const pbf &geom_pbf, const GlyphPositions &face, const Shaping &shaping);

public:
    const StyleBucketSymbol &properties;

private:
    Placement &placement;

    struct {
        TextVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<ElementGroup> groups;
    } text;

    struct {
        IconVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<ElementGroup> groups;
    } icon;

};
}

#endif
