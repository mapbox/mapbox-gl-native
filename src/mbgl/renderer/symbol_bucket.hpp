#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/text/glyph_range.hpp>

namespace mbgl {

class SDFShader;
class IconShader;
class CollisionBoxShader;
class GlyphAtlas;
class GlyphStore;
class SymbolRenderable;

class SymbolFeature {
public:
    GeometryCollection geometry;
    std::u32string label;
    std::string sprite;
    std::size_t index;
};

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
public:
    SymbolBucket(uint32_t overscaling, float zoom, const MapMode, const std::string& bucketName_, const std::string& sourceLayerName_);
    ~SymbolBucket() override;

    void upload(gl::ObjectStore&) override;
    void render(Painter&, const style::Layer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;
    bool needsClipping() const override;

    void addFeatures(uintptr_t tileUID,
                     SpriteAtlas&,
                     GlyphAtlas&,
                     GlyphStore&);

    void drawGlyphs(SDFShader&, gl::ObjectStore&);
    void drawIcons(SDFShader&, gl::ObjectStore&);
    void drawIcons(IconShader&, gl::ObjectStore&);
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

    void swapRenderable() override;

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

    std::unique_ptr<SymbolRenderable> renderable;
    std::unique_ptr<SymbolRenderable> renderableInProgress;
};

} // namespace mbgl
