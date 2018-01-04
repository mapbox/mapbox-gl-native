#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/text/bidi.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/programs/symbol_program.hpp>

#include <memory>
#include <map>
#include <unordered_set>
#include <vector>

namespace mbgl {

class BucketParameters;
class SymbolBucket;
class Anchor;
class RenderLayer;
class PlacedSymbol;

namespace style {
class Filter;
} // namespace style

class SymbolLayout {
public:
    SymbolLayout(const BucketParameters&,
                 const std::vector<const RenderLayer*>&,
                 std::unique_ptr<GeometryTileLayer>,
                 ImageDependencies&,
                 GlyphDependencies&);

    void prepare(const GlyphMap&, const GlyphPositions&,
                 const ImageMap&, const ImagePositions&,
                 const OverscaledTileID&, const std::string&);

    std::unique_ptr<SymbolBucket> place(const bool showCollisionBoxes);

    bool hasSymbolInstances() const;

    std::map<std::string,
        std::pair<style::IconPaintProperties::PossiblyEvaluated, style::TextPaintProperties::PossiblyEvaluated>> layerPaintProperties;

    const std::string bucketName;
    std::vector<SymbolInstance> symbolInstances;

private:
    void addFeature(const size_t,
                    const SymbolFeature&,
                    const std::pair<Shaping, Shaping>& shapedTextOrientations,
                    optional<PositionedIcon> shapedIcon,
                    const GlyphPositionMap&,
                    const OverscaledTileID&,
                    const std::string&);

    bool anchorIsTooClose(const std::u16string& text, const float repeatDistance, const Anchor&);
    std::map<std::u16string, std::vector<Anchor>> compareText;

    void addToDebugBuffers(SymbolBucket&);

    // Adds placed items to the buffer.
    template <typename Buffer>
    size_t addSymbol(Buffer&,
                   const Range<float> sizeData,
                   const SymbolQuad&,
                   const Anchor& labelAnchor,
                   PlacedSymbol& placedSymbol);

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
