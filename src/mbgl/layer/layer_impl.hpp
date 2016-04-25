#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/tile/geometry_tile.hpp>

#include <memory>
#include <string>
#include <limits>

namespace mbgl {

class StyleCascadeParameters;
class StyleCalculationParameters;
class StyleBucketParameters;
class Bucket;

/**
 * `Layer::Impl` contains the internal implementation of `Layer`: the details that need to be accessible to other parts
 * of the code, but hidden from the public API. Like `Layer`, it is an abstract base class, with derived classes for
 * each layer type.
 *
 * Members that are public in `Layer` are part of the public API for all layers.
 * Members that are public in `FooLayer` are part of the public API for "foo" layers.
 * Members that are public in `Layer::Impl` are part of the internal API for all layers.
 * Members that are public in `FooLayer::Impl` are part of the internal API for "foo" layers.
 * Members that are private in `FooLayer::Impl` are internal to "foo" layers.
 */
class Layer::Impl {
public:
    virtual ~Impl() = default;

    // Create an identical copy of this layer.
    virtual std::unique_ptr<Layer> clone() const = 0;

    virtual void parseLayout(const JSValue& value) = 0;
    virtual void parsePaints(const JSValue& value) = 0;

    // If the layer has a ref, the ref. Otherwise, the id.
    const std::string& bucketName() const;

    // Partially evaluate paint properties based on a set of classes.
    virtual void cascade(const StyleCascadeParameters&) = 0;

    // Fully evaluate cascaded paint properties based on a zoom level.
    // Returns true if any paint properties have active transitions.
    virtual bool recalculate(const StyleCalculationParameters&) = 0;

    virtual std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const = 0;

    // Checks whether this layer needs to be rendered in the given render pass.
    bool hasRenderPass(RenderPass) const;

    // Checks whether this layer can be rendered.
    bool needsRendering() const;

    virtual float getQueryRadius() const { return 0; }
    virtual bool queryIntersectsGeometry(
            const GeometryCollection&,
            const GeometryCollection&,
            const float,
            const float) const { return false; };

public:
    std::string id;
    std::string ref;
    std::string source;
    std::string sourceLayer;
    Filter filter;
    float minZoom = -std::numeric_limits<float>::infinity();
    float maxZoom = std::numeric_limits<float>::infinity();
    VisibilityType visibility = VisibilityType::Visible;

protected:
    Impl() = default;
    Impl(const Impl&) = default;
    Impl& operator=(const Impl&) = delete;

    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;
};

} // namespace mbgl
