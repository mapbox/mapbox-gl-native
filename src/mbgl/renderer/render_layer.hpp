#pragma once
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/mat4.hpp>

#include <memory>
#include <string>

namespace mbgl {

class Bucket;
class BucketParameters;
class TransitionParameters;
class PropertyEvaluationParameters;
class PaintParameters;
class RenderSource;
class RenderLayerSymbolInterface;
class RenderTile;
class TransformState;

class RenderLayer {
protected:
    RenderLayer(Immutable<style::Layer::Impl>);

public:
    virtual ~RenderLayer() = default;

    // Begin transitions for any properties that have changed since the last frame.
    virtual void transition(const TransitionParameters&) = 0;

    // Fully evaluate possibly-transitioning paint properties based on a zoom level.
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

    // Private implementation
    Immutable<style::Layer::Impl> baseImpl;
    void setImpl(Immutable<style::Layer::Impl>);

    virtual void markContextDestroyed();

    // TODO: Figure out how to remove this or whether layers other than
    // RenderHeatmapLayer and RenderLineLayer need paint property updates,
    // similar to color ramp. Temporarily moved to the base.
    virtual void update();

    // TODO: Only for background layers.
    virtual optional<Color> getSolidBackground() const;

protected:
    // Checks whether the current hardware can render this layer. If it can't, we'll show a warning
    // in the console to inform the developer.
    void checkRenderability(const PaintParameters&, uint32_t activeBindingCount);

    // For some layers, we want Buckets to cache their corresponding paint properties, so that outdated buckets (and
    // the cached paint properties) can be still in use while the tile is loading new buckets (which will
    // correpond to the current paint properties of the layer).
    virtual void updateBucketPaintProperties(Bucket*) const;
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
