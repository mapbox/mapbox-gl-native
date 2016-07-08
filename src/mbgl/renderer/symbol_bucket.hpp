#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/collision_box_buffer.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <memory>
#include <map>
#include <set>
#include <vector>

namespace mbgl {

class SDFShader;
class IconShader;
class CollisionBoxShader;
class CollisionTile;
class SpriteAtlas;
class SpriteStore;
class GlyphAtlas;
class GlyphStore;
class IndexedSubfeature;

class SymbolFeature {
public:
    GeometryCollection geometry;
    std::u32string label;
    std::string sprite;
    std::size_t index;
};

struct Anchor;

class SymbolInstance {
    public:
        explicit SymbolInstance(Anchor& anchor, const GeometryCoordinates& line,
                const Shaping& shapedText, const PositionedIcon& shapedIcon,
                const style::SymbolLayoutProperties&, const bool inside, const uint32_t index,
                const float textBoxScale, const float textPadding, const float textAlongLine,
                const float iconBoxScale, const float iconPadding, const float iconAlongLine,
                const GlyphPositions& face, const IndexedSubfeature& indexedfeature);
        Point<float> point;
        uint32_t index;
        bool hasText;
        bool hasIcon;
        SymbolQuads glyphQuads;
        SymbolQuads iconQuads;
        CollisionFeature textCollisionFeature;
        CollisionFeature iconCollisionFeature;
};

class SymbolBucket : public Bucket {
    typedef ElementGroup<2> TextElementGroup;
    typedef ElementGroup<4> IconElementGroup;
    typedef ElementGroup<1> CollisionBoxElementGroup;

public:
    SymbolBucket(uint32_t overscaling, float zoom, const MapMode, std::string bucketName_, std::string sourceLayerName_);
    ~SymbolBucket() override;

    void upload(gl::ObjectStore&, gl::Config&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;
    bool needsClipping() const override;

    void addFeatures(uintptr_t tileUID,
                     SpriteAtlas&,
                     GlyphAtlas&,
                     GlyphStore&);

    void drawGlyphs(SDFShader&, gl::ObjectStore&, bool overdraw);
    void drawIcons(SDFShader&, gl::ObjectStore&, bool overdraw);
    void drawIcons(IconShader&, gl::ObjectStore&, bool overdraw);
    void drawCollisionBoxes(CollisionBoxShader&, gl::ObjectStore&);

    void parseFeatures(const GeometryTileLayer&, const style::Filter&);
    bool needsDependencies(GlyphStore&, SpriteStore&);
    void placeFeatures(CollisionTile&) override;

private:
    void addFeature(const GeometryCollection &lines,
            const Shaping &shapedText, const PositionedIcon &shapedIcon,
            const GlyphPositions &face,
            const size_t index);
    bool anchorIsTooClose(const std::u32string &text, const float repeatDistance, Anchor &anchor);
    std::map<std::u32string, std::vector<Anchor>> compareText;

    void addToDebugBuffers(CollisionTile &collisionTile);

    void swapRenderData() override;

    // Adds placed items to the buffer.
    template <typename Buffer, typename GroupType>
    void addSymbols(Buffer &buffer, const SymbolQuads &symbols, float scale,
            const bool keepUpright, const bool alongLine, const float placementAngle);

public:
    style::SymbolLayoutProperties layout;

    float iconMaxSize = 1.0f;
    float textMaxSize = 16.0f;

    bool sdfIcons = false;
    bool iconsNeedLinear = false;

private:

    const float overscaling;
    const float zoom;
    const uint32_t tileSize;
    const float tilePixelRatio;
    const MapMode mode;
    const std::string bucketName;
    const std::string sourceLayerName;

    std::set<GlyphRange> ranges;
    std::vector<SymbolInstance> symbolInstances;
    std::vector<SymbolFeature> features;

    struct SymbolRenderData {
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

    std::unique_ptr<SymbolRenderData> renderData;
    std::unique_ptr<SymbolRenderData> renderDataInProgress;
};

} // namespace mbgl
