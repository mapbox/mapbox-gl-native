#ifndef MBGL_RENDERER_SYMBOLBUCKET
#define MBGL_RENDERER_SYMBOLBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/collision_box_buffer.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layout.hpp>

#include <memory>
#include <map>
#include <vector>

namespace mbgl {

class SDFShader;
class IconShader;
class CollisionBoxShader;
class DotShader;
class CollisionTile;
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

struct Anchor;

class SymbolInstance {
    public:
        explicit SymbolInstance(Anchor &anchor, const std::vector<Coordinate> &line,
                const Shaping &shapedText, const PositionedIcon &shapedIcon,
                const StyleLayoutSymbol &layout, const bool inside,
                const float textBoxScale, const float textPadding, const float textAlongLine,
                const float iconBoxScale, const float iconPadding, const float iconAlongLine,
                const GlyphPositions &face);
        const bool hasText;
        const bool hasIcon;
        const SymbolQuads glyphQuads;
        const SymbolQuads iconQuads;
        CollisionFeature textCollisionFeature;
        CollisionFeature iconCollisionFeature;
};

class SymbolBucket : public Bucket {
    typedef ElementGroup<1> TextElementGroup;
    typedef ElementGroup<2> IconElementGroup;
    typedef ElementGroup<1> CollisionBoxElementGroup;

public:
    SymbolBucket(CollisionTile &collision);
    ~SymbolBucket() override;

    void render(Painter &painter, const StyleLayer &layer_desc, const TileID &id,
                const mat4 &matrix) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;

    void addFeatures(const GeometryTileLayer&,
                     const FilterExpression&,
                     uintptr_t tileUID,
                     SpriteAtlas&,
                     Sprite&,
                     GlyphAtlas&,
                     GlyphStore&);

    void drawGlyphs(SDFShader& shader);
    void drawIcons(SDFShader& shader);
    void drawIcons(IconShader& shader);
    void drawCollisionBoxes(CollisionBoxShader& shader);

private:
    std::vector<SymbolFeature> processFeatures(const GeometryTileLayer&,
                                               const FilterExpression&,
                                               GlyphStore&,
                                               const Sprite&);
    void addFeature(const std::vector<std::vector<Coordinate>> &lines,
            const Shaping &shapedText, const PositionedIcon &shapedIcon,
            const GlyphPositions &face);

    void placeFeatures();
    void addToDebugBuffers();

    // Adds placed items to the buffer.
    template <typename Buffer, typename GroupType>
    void addSymbols(Buffer &buffer, const SymbolQuads &symbols, float scale, const bool keepUpright, const bool alongLine);

public:
    StyleLayoutSymbol layout;
    bool sdfIcons = false;

private:
    CollisionTile &collision;
    std::vector<SymbolInstance> symbolInstances;

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

    struct CollisionBoxBuffer {
        CollisionBoxVertexBuffer vertices;
        std::vector<std::unique_ptr<CollisionBoxElementGroup>> groups;
    } collisionBox;

};
}

#endif
