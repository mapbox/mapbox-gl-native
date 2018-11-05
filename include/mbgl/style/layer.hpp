#pragma once

#include <mbgl/util/peer.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/style/layer_type.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/conversion.hpp>

#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>

namespace mbgl {
namespace style {

class LayerObserver;
class Filter;

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
 * type, calling `LayerManager`:
 *
 *     auto circleLayer = LayerManager::get()->createLayer("circle", ...);
 */
class Layer {
public:
    Layer(const Layer& ) = delete;
    Layer& operator=(const Layer&) = delete;

    virtual ~Layer();
    // Note: LayerType is deprecated, do not use it.
    LayerType getType() const;
    std::string getID() const;
    // Source
    std::string getSourceID() const;
    std::string getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    // Filter
    const Filter& getFilter() const;
    void setFilter(const Filter&);

    // Visibility
    VisibilityType getVisibility() const;
    void setVisibility(VisibilityType);

    // Zoom range
    float getMinZoom() const;
    float getMaxZoom() const;
    void setMinZoom(float);
    void setMaxZoom(float);

    // Dynamic properties
    virtual optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) = 0;
    virtual optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) = 0;
    optional<conversion::Error> setVisibility(const conversion::Convertible& value);

    // Private implementation
    // TODO : We should not have public mutable data members.
    class Impl;
    Immutable<Impl> baseImpl;

    // Create a layer, copying all properties except id and paint properties from this layer.
    virtual std::unique_ptr<Layer> cloneRef(const std::string& id) const = 0;

    
    void setObserver(LayerObserver*);

    // For use in SDK bindings, which store a reference to a platform-native peer
    // object here, so that separately-obtained references to this object share
    // identical platform-native peers.
    util::peer peer;
    Layer(Immutable<Impl>);
protected:
    virtual Mutable<Impl> mutableBaseImpl() const = 0;

    LayerObserver* observer;
};

/**
 * @brief The LayerFactory abstract class
 * 
 * This class is responsible for creation of the layer objects that belong to a concrete layer type.
 */
class LayerFactory {
public:
    virtual ~LayerFactory() = default;
    /// Returns \c true if this factory can produce layers of the given type of the layers; returns \c false otherwise.
    virtual bool supportsType(const std::string& type) const = 0;
    /// Returns a new Layer instance on success call; returns `nulltptr` otherwise. 
    virtual std::unique_ptr<Layer> createLayer(const std::string& id, const conversion::Convertible& value) = 0;

protected:
    optional<std::string> getSource(const conversion::Convertible& value) const;
    bool initSourceLayerAndFilter(Layer*, const conversion::Convertible& value) const;
};

/**
 * @brief A singleton class forwarding calls to the corresponding  \c LayerFactory instance.
 */
class LayerManager {
public:
    /**
     * @brief A singleton getter.
     * 
     * @return LayerManager* 
     */
    static LayerManager* get();

    virtual ~LayerManager() = default;
    /// Returns a new Layer instance on success call; returns `nulltptr` otherwise.
    virtual std::unique_ptr<Layer> createLayer(const std::string& type, const std::string& id, const conversion::Convertible& value, conversion::Error& error) = 0;

protected:
    LayerManager() = default;
};

} // namespace style
} // namespace mbgl
