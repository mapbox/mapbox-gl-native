#ifndef MBGL_RENDERER_SYMBOLBUCKET
#define MBGL_RENDERER_SYMBOLBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/text/types.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layout.hpp>

#include <memory>
#include <map>
#include <vector>

namespace mbgl {

class SDFShader;
class IconShader;
class Collision;
class SpriteAtlas;
class Sprite;
class GlyphAtlas;
class GlyphStore;

class SymbolFeature {
public:
    std::vector<std::vector<Coordinate>> geometry;
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
    typedef ElementGroup<2> IconElementGroup;

public:
    SymbolBucket(Collision &collision);
    ~SymbolBucket() override;

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;
    bool hasData() const;
    bool hasTextData() const;
    bool hasIconData() const;

    void addFeatures(uintptr_t tileUID,
                     SpriteAtlas&,
                     Sprite&,
                     GlyphAtlas&,
                     GlyphStore&);

    void drawGlyphs(SDFShader& shader);
    void drawIcons(SDFShader& shader);
    void drawIcons(IconShader& shader);

    bool needsDependencies(const GeometryTileLayer&,
                           const FilterExpression&,
                           GlyphStore&,
                           Sprite&);

private:
    void addFeature(const std::vector<Coordinate> &line, const Shaping &shaping, const GlyphPositions &face, const Rect<uint16_t> &image);

    // Adds placed items to the buffer.
    template <typename Buffer, typename GroupType>
    void addSymbols(Buffer &buffer, const PlacedGlyphs &symbols, float scale, PlacementRange placementRange);

public:
    StyleLayoutSymbol layout;
    bool sdfIcons = false;

private:
    Collision &collision;
    std::vector<SymbolFeature> features;

    struct TextBuffer {
        TextVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<std::unique_ptr<TextElementGroup>> groups;
    } text;

    struct IconBuffer {
        IconVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<std::unique_ptr<IconElementGroup>> groups;
    } icon;
};

}

#endif
