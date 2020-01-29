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
                 const LayoutParameters& parameters);

    ~SymbolLayout() final = default;

    void prepareSymbols(const GlyphMap& glyphMap,
                        const GlyphPositions&,
                        const ImageMap&,
                        const ImagePositions&) override;

    void createBucket(const ImagePositions&,
                      std::unique_ptr<FeatureIndex>&,
                      std::unordered_map<std::string, LayerRenderData>&,
                      const bool firstLoad,
                      const bool showCollisionBoxes,
                      const CanonicalTileID& canonical) override;

    bool hasSymbolInstances() const override;
    bool hasDependencies() const override;

    std::map<std::string, Immutable<style::LayerProperties>> layerPaintProperties;

    const std::string bucketLeaderID;
    std::vector<SymbolInstance> symbolInstances;

    static constexpr float INVALID_OFFSET_VALUE = std::numeric_limits<float>::max();
    /**
     * @brief Calculates variable text offset.
     * 
     * @param anchor text anchor
     * @param textOffset Either `text-offset` or [ `text-radial-offset`, INVALID_OFFSET_VALUE ]
     * @return std::array<float, 2> offset along x- and y- axis correspondingly.
     */
    static std::array<float, 2> evaluateVariableOffset(style::SymbolAnchorType anchor, std::array<float, 2> textOffset);

    static std::vector<float> calculateTileDistances(const GeometryCoordinates& line, const Anchor& anchor);

private:
    void addFeature(const size_t,
                    const SymbolFeature&,
                    const ShapedTextOrientations& shapedTextOrientations,
                    optional<PositionedIcon> shapedIcon,
                    const ImageMap&,
                    std::array<float, 2> textOffset,
                    float layoutTextSize,
                    float layoutIconSize,
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
    size_t addSymbols(SymbolBucket::Buffer&,
                      const Range<float> sizeData,
                      const SymbolQuads&,
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
                                    const CanonicalTileID& canonical,
                                    optional<std::size_t> lastAddedSection = nullopt);

    void updatePaintPropertiesForSection(SymbolBucket&,
                                         const SymbolFeature&,
                                         std::size_t sectionIndex,
                                         const CanonicalTileID& canonical);

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
    bool iconsInText = false;
    std::vector<style::TextWritingModeType> placementModes;

    style::TextSize::UnevaluatedType textSize;
    style::IconSize::UnevaluatedType iconSize;
    style::TextRadialOffset::UnevaluatedType textRadialOffset;
    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout;
    std::vector<SymbolFeature> features;

    BiDi bidi; // Consider moving this up to geometry tile worker to reduce reinstantiation costs; use of BiDi/ubiditransform object must be constrained to one thread
};

} // namespace mbgl
