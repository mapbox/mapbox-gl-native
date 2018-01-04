#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/filter.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

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
    Impl(LayerType, std::string layerID, std::string sourceID);
    virtual ~Impl() = default;

    Impl& operator=(const Impl&) = delete;

    // Returns true buckets if properties affecting layout have changed: i.e. filter,
    // visibility, layout properties, or data-driven paint properties.
    virtual bool hasLayoutDifference(const Layer::Impl&) const = 0;

    // Utility function for automatic layer grouping.
    virtual void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const = 0;

    const LayerType type;
    std::string id;
    std::string source;
    std::string sourceLayer;
    Filter filter;
    float minZoom = -std::numeric_limits<float>::infinity();
    float maxZoom = std::numeric_limits<float>::infinity();
    VisibilityType visibility = VisibilityType::Visible;

protected:
    Impl(const Impl&) = default;
};

} // namespace style
} // namespace mbgl
