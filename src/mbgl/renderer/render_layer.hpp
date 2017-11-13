#pragma once

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_type.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <memory>
#include <string>

namespace mbgl {

class Bucket;
class BucketParameters;
class TransitionParameters;
class PropertyEvaluationParameters;
class PaintParameters;
class RenderSource;
class RenderTile;

class RenderLayer {
protected:
    RenderLayer(style::LayerType, Immutable<style::Layer::Impl>);

    const style::LayerType type;

public:
    static std::unique_ptr<RenderLayer> create(Immutable<style::Layer::Impl>);

    virtual ~RenderLayer() = default;

    // Begin transitions for any properties that have changed since the last frame.
    virtual void transition(const TransitionParameters&) = 0;

    // Fully evaluate possibly-transitioning paint properties based on a zoom level.
    virtual void evaluate(const PropertyEvaluationParameters&) = 0;

    // Returns true if any paint properties have active transitions.
    virtual bool hasTransition() const = 0;

    // Check whether this layer is of the given subtype.
    template <class T>
    bool is() const;

    // Dynamically cast this layer to the given subtype.
    template <class T>
    T* as() {
        return is<T>() ? reinterpret_cast<T*>(this) : nullptr;
    }

    template <class T>
    const T* as() const {
        return is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
    }

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
            const float,
            const float) const { return false; };

    virtual std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const = 0;

    void setRenderTiles(std::vector<std::reference_wrapper<RenderTile>>);
    // Private implementation
    Immutable<style::Layer::Impl> baseImpl;
    void setImpl(Immutable<style::Layer::Impl>);

    friend std::string layoutKey(const RenderLayer&);

protected:
    // renderTiles are exposed directly to CrossTileSymbolIndex and Placement so they
    // can update opacities in the symbol buckets immediately before rendering
    friend class CrossTileSymbolIndex;
    friend class Placement;
    // Stores current set of tiles to be rendered for this layer.
    std::vector<std::reference_wrapper<RenderTile>> renderTiles;

    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;
};

} // namespace mbgl
