#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/unique_any.hpp>
#include <mbgl/util/immutable.hpp>
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
class HillshadeLayer;
class BackgroundLayer;
class CustomLayer;
class FillExtrusionLayer;
class HeatmapLayer;
class LayerObserver;

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
        switch (getType()) {
        case LayerType::Fill:
            return std::forward<V>(visitor)(*as<FillLayer>());
        case LayerType::Line:
            return std::forward<V>(visitor)(*as<LineLayer>());
        case LayerType::Circle:
            return std::forward<V>(visitor)(*as<CircleLayer>());
        case LayerType::Symbol:
            return std::forward<V>(visitor)(*as<SymbolLayer>());
        case LayerType::Raster:
            return std::forward<V>(visitor)(*as<RasterLayer>());
        case LayerType::Background:
            return std::forward<V>(visitor)(*as<BackgroundLayer>());
        case LayerType::Hillshade:
            return std::forward<V>(visitor)(*as<HillshadeLayer>());
        case LayerType::Custom:
            return std::forward<V>(visitor)(*as<CustomLayer>());
        case LayerType::FillExtrusion:
            return std::forward<V>(visitor)(*as<FillExtrusionLayer>());
        case LayerType::Heatmap:
            return std::forward<V>(visitor)(*as<HeatmapLayer>());
        }


        // Not reachable, but placate GCC.
        assert(false);
        throw new std::runtime_error("unknown layer type");
    }

    LayerType getType() const;
    std::string getID() const;

    // Visibility
    VisibilityType getVisibility() const;
    virtual void setVisibility(VisibilityType) = 0;

    // Zoom range
    float getMinZoom() const;
    float getMaxZoom() const;
    virtual void setMinZoom(float) = 0;
    virtual void setMaxZoom(float) = 0;

    // Private implementation
    class Impl;
    Immutable<Impl> baseImpl;

    Layer(Immutable<Impl>);

    // Create a layer, copying all properties except id and paint properties from this layer.
    virtual std::unique_ptr<Layer> cloneRef(const std::string& id) const = 0;

    LayerObserver* observer = nullptr;
    void setObserver(LayerObserver*);

    // For use in SDK bindings, which store a reference to a platform-native peer
    // object here, so that separately-obtained references to this object share
    // identical platform-native peers.
    util::unique_any peer;
};

} // namespace style
} // namespace mbgl
