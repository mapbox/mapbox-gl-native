#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/layout/symbol_instance.hpp>

#include <memory>
#include <map>
#include <unordered_set>
#include <vector>

namespace mbgl {

class GeometryTileLayer;
class CollisionTile;
class SpriteAtlas;
class GlyphAtlas;
class SymbolBucket;

namespace style {
class Filter;
} // namespace style

struct Anchor;

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

    bool canPrepare(GlyphAtlas&);

    void prepare(uintptr_t tileUID,
                 GlyphAtlas&);

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
    void addSymbols(Buffer&, const SymbolQuads&, float scale,
                    const bool keepUpright, const style::SymbolPlacementType, const float placementAngle);

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

    GlyphRangeSet ranges;
    std::vector<SymbolInstance> symbolInstances;
    std::vector<SymbolFeature> features;
};

} // namespace mbgl
