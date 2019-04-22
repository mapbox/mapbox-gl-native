#pragma once
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/mat4.hpp>

#include <memory>
#include <string>

namespace mbgl {

class Bucket;
class TransitionParameters;
class PropertyEvaluationParameters;
class PaintParameters;
class RenderSource;
class RenderLayerSymbolInterface;
class RenderTile;
class TransformState;

class LayerRenderData {
public:
    std::shared_ptr<Bucket> bucket;
    Immutable<style::LayerProperties> layerProperties;
};

class RenderLayer {
protected:
    RenderLayer(Immutable<style::LayerProperties>);

public:
    virtual ~RenderLayer() = default;

    // Begin transitions for any properties that have changed since the last frame.
    virtual void transition(const TransitionParameters&) = 0;

    // Overloaded version for transitions to a new layer impl.
    void transition(const TransitionParameters&, Immutable<style::Layer::Impl> newImpl);

    // Fully evaluate possibly-transitioning paint properties based on a zoom level.
    // Updates the contained `evaluatedProperties` member.
    virtual void evaluate(const PropertyEvaluationParameters&) = 0;

    // Returns true if any paint properties have active transitions.
    virtual bool hasTransition() const = 0;

    // Returns true if the layer has a pattern property and is actively crossfading.
    virtual bool hasCrossfade() const = 0;

    // Returns instance of RenderLayerSymbolInterface if RenderLayer supports it.
    virtual const RenderLayerSymbolInterface* getSymbolInterface() const;

    const std::string& getID() const;

    // Checks whether this layer needs to be rendered in the given render pass.
    bool hasRenderPass(RenderPass) const;

    // Checks whether this layer can be rendered.
    bool needsRendering(float zoom) const;

    virtual void render(PaintParameters&, RenderSource*) = 0;

    // Check wether the given geometry intersects
    // with the feature
    virtual bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const { return false; };

    using RenderTiles = std::vector<std::reference_wrapper<RenderTile>>;
    virtual void setRenderTiles(RenderTiles, const TransformState&);

    // Latest evaluated properties.
    Immutable<style::LayerProperties> evaluatedProperties;
    // Private implementation
    Immutable<style::Layer::Impl> baseImpl;

    virtual void markContextDestroyed();

    // TODO: Only for background layers.
    virtual optional<Color> getSolidBackground() const;

protected:
    // Checks whether the current hardware can render this layer. If it can't, we'll show a warning
    // in the console to inform the developer.
    void checkRenderability(const PaintParameters&, uint32_t activeBindingCount);

    using FilterFunctionPtr = bool (*)(RenderTile&);
    RenderTiles filterRenderTiles(RenderTiles, FilterFunctionPtr) const;

protected:
    // Stores current set of tiles to be rendered for this layer.
    std::vector<std::reference_wrapper<RenderTile>> renderTiles;

    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;

private:
    // Some layers may not render correctly on some hardware when the vertex attribute limit of
    // that GPU is exceeded. More attributes are used when adding many data driven paint properties
    // to a layer.
    bool hasRenderFailures = false;
};

} // namespace mbgl
