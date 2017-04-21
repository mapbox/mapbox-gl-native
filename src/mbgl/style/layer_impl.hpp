#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <memory>
#include <string>
#include <limits>

namespace mbgl {

class RenderLayer;

namespace style {

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

    // Create a new layer with the specified `id` and `sourceID`. All other properties
    // are copied from this layer.
    std::unique_ptr<Layer> copy(const std::string& id,
                                const std::string& sourceID) const;

    // Create an identical copy of this layer.
    virtual std::unique_ptr<Layer> clone() const = 0;

    // Create a layer, copying all properties except id and paint properties from this layer.
    virtual std::unique_ptr<Layer> cloneRef(const std::string& id) const = 0;

    // Utility function for automatic layer grouping.
    virtual void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const = 0;

    virtual std::unique_ptr<RenderLayer> createRenderLayer() const = 0;

    void setObserver(LayerObserver*);

public:
    std::string id;
    std::string source;
    std::string sourceLayer;
    Filter filter;
    float minZoom = -std::numeric_limits<float>::infinity();
    float maxZoom = std::numeric_limits<float>::infinity();
    VisibilityType visibility = VisibilityType::Visible;

    LayerObserver nullObserver;
    LayerObserver* observer = &nullObserver;

protected:
    Impl() = default;
    Impl(const Impl&) = default;
    Impl& operator=(const Impl&) = delete;
};

} // namespace style
} // namespace mbgl
