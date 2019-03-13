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
class RenderLayer;
class PlacedSymbol;

namespace style {
class Filter;
} // namespace style

class SymbolLayout final : public Layout {
public:
    SymbolLayout(const BucketParameters&,
                 const std::vector<const RenderLayer*>&,
                 std::unique_ptr<GeometryTileLayer>,
                 ImageDependencies&,
                 GlyphDependencies&);
    
    ~SymbolLayout() final = default;

    void prepareSymbols(const GlyphMap&, const GlyphPositions&,
                 const ImageMap&, const ImagePositions&) override;

    void createBucket(const ImagePositions&, std::unique_ptr<FeatureIndex>&, std::unordered_map<std::string, std::shared_ptr<Bucket>>&, const bool firstLoad, const bool showCollisionBoxes) override;

    bool hasSymbolInstances() const override;
    bool hasDependencies() const override;

    std::map<std::string, style::SymbolPaintProperties::PossiblyEvaluated> layerPaintProperties;

    const std::string bucketLeaderID;
    std::vector<SymbolInstance> symbolInstances;

private:
    void addFeature(const size_t,
                    const SymbolFeature&,
                    const std::pair<Shaping, Shaping>& shapedTextOrientations,
                    optional<PositionedIcon> shapedIcon,
                    const GlyphPositions&);

    bool anchorIsTooClose(const std::u16string& text, const float repeatDistance, const Anchor&);
    std::map<std::u16string, std::vector<Anchor>> compareText;

    void addToDebugBuffers(SymbolBucket&);

    // Adds placed items to the buffer.
    size_t addSymbol(SymbolBucket::Buffer&,
                     const Range<float> sizeData,
                     const SymbolQuad&,
                     const Anchor& labelAnchor,
                     PlacedSymbol& placedSymbol);

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

    style::SymbolLayoutProperties::PossiblyEvaluated layout;

    const uint32_t tileSize;
    const float tilePixelRatio;

    bool sdfIcons = false;
    bool iconsNeedLinear = false;

    style::TextSize::UnevaluatedType textSize;
    style::IconSize::UnevaluatedType iconSize;

    std::vector<SymbolFeature> features;

    BiDi bidi; // Consider moving this up to geometry tile worker to reduce reinstantiation costs; use of BiDi/ubiditransform object must be constrained to one thread
};

} // namespace mbgl
