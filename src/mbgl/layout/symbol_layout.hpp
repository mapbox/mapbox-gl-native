#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <memory>
#include <map>
#include <set>
#include <vector>

namespace mbgl {

class CollisionTile;
class SpriteAtlas;
class SpriteStore;
class GlyphAtlas;
class GlyphStore;
class IndexedSubfeature;
class SymbolBucket;

namespace style {
class Filter;
} // namespace style

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
    explicit SymbolInstance(Anchor& anchor,
                            const GeometryCoordinates& line,
                            const Shaping& shapedText,
                            const PositionedIcon& shapedIcon,
                            const style::SymbolLayoutProperties&,
                            const bool inside,
                            const uint32_t index,
                            const float textBoxScale,
                            const float textPadding,
                            style::SymbolPlacementType textPlacement,
                            const float iconBoxScale,
                            const float iconPadding,
                            style::SymbolPlacementType iconPlacement,
                            const GlyphPositions& face,
                            const IndexedSubfeature& indexedfeature);

    Point<float> point;
    uint32_t index;
    bool hasText;
    bool hasIcon;
    SymbolQuads glyphQuads;
    SymbolQuads iconQuads;
    CollisionFeature textCollisionFeature;
    CollisionFeature iconCollisionFeature;
};

class SymbolLayout {
public:
    SymbolLayout(std::string bucketName_,
                 std::string sourceLayerName_,
                 uint32_t overscaling,
                 float zoom,
                 const MapMode,
                 const GeometryTileLayer&,
                 const style::Filter&,
                 style::SymbolLayoutProperties,
                 float textMaxSize,
                 SpriteAtlas&);

    bool canPrepare(GlyphStore&, SpriteStore&);

    void prepare(uintptr_t tileUID, GlyphAtlas&, GlyphStore&);

    std::unique_ptr<SymbolBucket> place(CollisionTile&);

    bool hasSymbolInstances() const;

    enum State {
        Pending,  // Waiting for the necessary glyphs or icons to be available.
        Prepared, // The potential positions of text and icons have been determined.
        Placed    // The final positions have been determined, taking into account prior layers.
    };

    State state = Pending;

    const std::string bucketName;
    const std::string sourceLayerName;

private:
    void addFeature(const GeometryCollection&,
                    const Shaping& shapedText,
                    const PositionedIcon& shapedIcon,
                    const GlyphPositions& face,
                    const size_t index);

    bool anchorIsTooClose(const std::u32string& text, const float repeatDistance, Anchor&);
    std::map<std::u32string, std::vector<Anchor>> compareText;

    void addToDebugBuffers(CollisionTile&, SymbolBucket&);

    // Adds placed items to the buffer.
    template <typename Buffer, typename GroupType>
    void addSymbols(Buffer&,
                    const SymbolQuads&,
                    float scale,
                    const bool keepUpright,
                    const style::SymbolPlacementType,
                    const float placementAngle);

    const float overscaling;
    const float zoom;
    const MapMode mode;
    const style::SymbolLayoutProperties layout;
    const float textMaxSize;

    SpriteAtlas& spriteAtlas;

    const uint32_t tileSize;
    const float tilePixelRatio;

    bool sdfIcons = false;
    bool iconsNeedLinear = false;

    std::set<GlyphRange> ranges;
    std::vector<SymbolInstance> symbolInstances;
    std::vector<SymbolFeature> features;
};

} // namespace mbgl
