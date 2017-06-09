#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/any.hpp>
#include <mbgl/style/layer_type.hpp>
#include <mbgl/style/types.hpp>

#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>

namespace mbgl {
namespace style {

class FillLayer;
class LineLayer;
class CircleLayer;
class SymbolLayer;
class RasterLayer;
class BackgroundLayer;
class CustomLayer;
class FillExtrusionLayer;

/**
 * The runtime representation of a [layer](https://www.mapbox.com/mapbox-gl-style-spec/#layers) from the Mapbox Style
 * Specification.
 *
 * `Layer` is an abstract base class; concrete derived classes are provided for each layer type. `Layer` contains
 * functionality that is common to all layer types:
 *
 * * Runtime type information: type predicates and casting
 * * Accessors for properties common to all layer types: ID, visibility, etc.
 * * Cloning and copying
 *
 * All other functionality lives in the derived classes. To instantiate a layer, create an instance of the desired
 * type, passing the ID:
 *
 *     auto circleLayer = std::make_unique<CircleLayer>("my-circle-layer");
 */
class Layer : public mbgl::util::noncopyable {
public:
    class Impl;

protected:

    const LayerType type;
    Layer(LayerType, std::unique_ptr<Impl>);

public:

    virtual ~Layer();

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

    // Convenience method for dynamic dispatch on the concrete layer type. Using
    // method overloading, this allows consolidation of logic common to vector-based
    // layers (Fill, FillExtrusion, Line, Circle, or Symbol). For example:
    //
    //     struct Visitor {
    //         void operator()(CustomLayer&) { ... }
    //         void operator()(RasterLayer&) { ... }
    //         void operator()(BackgroundLayer&) { ... }
    //         template <class VectorLayer>
    //         void operator()(VectorLayer&) { ... }
    //     };
    //
    template <class V>
    auto accept(V&& visitor) {
        switch (type) {
        case LayerType::Fill:
            return visitor(*as<FillLayer>());
        case LayerType::Line:
            return visitor(*as<LineLayer>());
        case LayerType::Circle:
            return visitor(*as<CircleLayer>());
        case LayerType::Symbol:
            return visitor(*as<SymbolLayer>());
        case LayerType::Raster:
            return visitor(*as<RasterLayer>());
        case LayerType::Background:
            return visitor(*as<BackgroundLayer>());
        case LayerType::Custom:
            return visitor(*as<CustomLayer>());
        case LayerType::FillExtrusion:
            return visitor(*as<FillExtrusionLayer>());
        }


        // Not reachable, but placate GCC.
        assert(false);
        throw new std::runtime_error("unknown layer type");
    }

    const std::string& getID() const;

    // Visibility
    VisibilityType getVisibility() const;
    void setVisibility(VisibilityType);

    // Zoom range
    float getMinZoom() const;
    void setMinZoom(float) const;
    float getMaxZoom() const;
    void setMaxZoom(float) const;

    // Private implementation
    const std::unique_ptr<Impl> baseImpl;

    // For use in SDK bindings, which store a reference to a platform-native peer
    // object here, so that separately-obtained references to this object share
    // identical platform-native peers.
    any peer;
};

} // namespace style
} // namespace mbgl
