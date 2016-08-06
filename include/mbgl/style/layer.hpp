#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/style/types.hpp>

#include <memory>

namespace mbgl {
namespace style {

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
    class Impl;
    const std::unique_ptr<Impl> baseImpl;

protected:
    enum class Type {
        Fill,
        Line,
        Circle,
        Symbol,
        Raster,
        Background,
        Custom,
    };

    const Type type;
    Layer(Type, std::unique_ptr<Impl>);
};

} // namespace style
} // namespace mbgl
