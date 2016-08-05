#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/style/types.hpp>

#include <memory>
#include <string>

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
protected:
    enum class Type {
        Fill,
        Line,
        Circle,
        Symbol,
        Raster,
        Background,
        Custom,
        FillExtrusion,
    };

    class Impl;
    const Type type;
    Layer(Type, std::unique_ptr<Impl>);

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
    // layers (Fill, Line, Circle, or Symbol). For example:
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
        case Type::Fill:
            return visitor(*as<FillLayer>());
        case Type::Line:
            return visitor(*as<LineLayer>());
        case Type::Circle:
            return visitor(*as<CircleLayer>());
        case Type::Symbol:
            return visitor(*as<SymbolLayer>());
        case Type::Raster:
            return visitor(*as<RasterLayer>());
        case Type::Background:
            return visitor(*as<BackgroundLayer>());
        case Type::Custom:
            return visitor(*as<CustomLayer>());
        case Type::FillExtrusion:
            return visitor(*as<FillExtrusionLayer>());
        }
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

    friend std::string layoutKey(const Layer&);
};

} // namespace style
} // namespace mbgl
