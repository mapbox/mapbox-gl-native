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
class CascadeParameters;
class PropertyEvaluationParameters;

class RenderLayer {

protected:
    RenderLayer(style::LayerType, const style::Layer::Impl&);

    const style::LayerType type;

public:

    virtual ~RenderLayer() = default;

    // Create an identical copy of this layer.
    virtual std::unique_ptr<RenderLayer> clone() const = 0;

    // Partially evaluate paint properties based on a set of classes.
    virtual void cascade(const CascadeParameters&) = 0;

    // Fully evaluate cascaded paint properties based on a zoom level.
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

    // Check wether the given geometry intersects
    // with the feature
    virtual bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const float,
            const float) const { return false; };

    virtual std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const = 0;

    // Private implementation
    const style::Layer::Impl& baseImpl;

    friend std::string layoutKey(const RenderLayer&);
protected:

    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;
};

} // namespace mbgl
