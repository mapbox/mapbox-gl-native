#pragma once

#include <mbgl/layout/layout.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/text/bidi.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>

#include <memory>
#include <map>
#include <vector>

namespace mbgl {

class BucketParameters;
class Anchor;
class PlacedSymbol;

namespace style {
class Filter;
} // namespace style

class SymbolLayout final : public Layout {
public:
    SymbolLayout(const BucketParameters&,
                 const std::vector<Immutable<style::LayerProperties>>&,
                 std::unique_ptr<GeometryTileLayer>,
                 ImageDependencies&,
                 GlyphDependencies&);
    
    ~SymbolLayout() final = default;

    void prepareSymbols(const GlyphMap&, const GlyphPositions&,
                 const ImageMap&, const ImagePositions&) override;

    void createBucket(const ImagePositions&, std::unique_ptr<FeatureIndex>&, std::unordered_map<std::string, LayerRenderData>&, const bool firstLoad, const bool showCollisionBoxes) override;

    bool hasSymbolInstances() const override;
    bool hasDependencies() const override;

    std::map<std::string, Immutable<style::LayerProperties>> layerPaintProperties;

    const std::string bucketLeaderID;
    std::vector<SymbolInstance> symbolInstances;

    static Point<float> evaluateRadialOffset(style::SymbolAnchorType anchor, float radialOffset);

private:
    void addFeature(const size_t,
                    const SymbolFeature&,
                    const ShapedTextOrientations& shapedTextOrientations,
                    optional<PositionedIcon> shapedIcon,
                    const GlyphPositions&,
                    Point<float> textOffset,
                    const SymbolContent iconType);

    bool anchorIsTooClose(const std::u16string& text, const float repeatDistance, const Anchor&);
    std::map<std::u16string, std::vector<Anchor>> compareText;

    void addToDebugBuffers(SymbolBucket&);

    // Adds placed items to the buffer.
    size_t addSymbol(SymbolBucket::Buffer&,
                     const Range<float> sizeData,
                     const SymbolQuad&,
                     const Anchor& labelAnchor,
                     PlacedSymbol& placedSymbol,
                     float sortKey);

    // Adds symbol quads to bucket and returns formatted section index of last
    // added quad.
    std::size_t addSymbolGlyphQuads(SymbolBucket&,
                                    SymbolInstance&,
                                    const SymbolFeature&,
                                    WritingModeType,
                                    optional<size_t>& placedIndex,
                                    const SymbolQuads&,
                                    optional<std::size_t> lastAddedSection = nullopt);

    void updatePaintPropertiesForSection(SymbolBucket&,
                                         const SymbolFeature&,
                                         std::size_t sectionIndex);

    // Stores the layer so that we can hold on to GeometryTileFeature instances in SymbolFeature,
    // which may reference data from this object.
    const std::unique_ptr<GeometryTileLayer> sourceLayer;
    const float overscaling;
    const float zoom;
    const MapMode mode;
    const float pixelRatio;

    const uint32_t tileSize;
    const float tilePixelRatio;

    bool iconsNeedLinear = false;
    bool sortFeaturesByY = false;
    bool allowVerticalPlacement = false;
    std::vector<style::TextWritingModeType> placementModes;

    style::TextSize::UnevaluatedType textSize;
    style::IconSize::UnevaluatedType iconSize;
    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout;
    std::vector<SymbolFeature> features;

    BiDi bidi; // Consider moving this up to geometry tile worker to reduce reinstantiation costs; use of BiDi/ubiditransform object must be constrained to one thread
};

} // namespace mbgl
