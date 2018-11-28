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
 * @brief Holds static data for a certain layer type.
 */
struct LayerTypeInfo {
    /**
     * @brief contains the layer type as defined in the style specification;
     */
    const char* type;

    /**
     * @brief contains \c Source::Required if the corresponding layer type
     * requires source. Contains \c Source::NotRequired otherwise.
     */
    const enum class Source { Required, NotRequired } source;

    /**
     * @brief contains \c Pass3D::Required if the corresponding layer type
     * requires 3D rendering pass. Contains \c Pass3D::NotRequired otherwise.
     */
    const enum class Pass3D { Required, NotRequired } pass3d;

    /**
     * @brief contains \c Layout::Required if the corresponding layer type
     * requires layouting. * contains \c Layout::NotRequired otherwise.
     */
    const enum class Layout { Required, NotRequired } layout;

    /**
     * @brief contains \c Clipping::Required if the corresponding layer type
     * requires clipping. Contains \c Clipping::NotRequired otherwise.
     */
    const enum class Clipping { Required, NotRequired } clipping;
};

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

    const LayerTypeInfo* getTypeInfo() const noexcept;

protected:
    virtual Mutable<Impl> mutableBaseImpl() const = 0;

    LayerObserver* observer;
};

} // namespace style

class RenderLayer;
// TODO: The following classes shall not be here. Move layer factories and manager to a dedicated folder.

/**
 * @brief The LayerFactory abstract class
 * 
 * This class is responsible for creation of the layer objects that belong to a concrete layer type.
 */
class LayerFactory {
public:
    virtual ~LayerFactory() = default;
    /// Returns the layer type data.
    virtual const style::LayerTypeInfo* getTypeInfo() const noexcept = 0;
    /// Returns a new Layer instance on success call; returns `nulltptr` otherwise. 
    virtual std::unique_ptr<style::Layer> createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept = 0;
    /// Returns a new RenderLayer instance on success call; returns `nulltptr` otherwise.
    virtual std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept = 0;

protected:
    optional<std::string> getSource(const style::conversion::Convertible& value) const noexcept;
    bool initSourceLayerAndFilter(style::Layer*, const style::conversion::Convertible& value) const noexcept;
};

/**
 * @brief A singleton class responsible for creating layer instances.
 * 
 * The LayerManager has implementation per platform. The LayerManager implementation
 * defines what layer types are available and it can also disable annotations.
 * 
 * Linker excludes the unreachable code for the disabled annotations and layers
 * from the binaries, significantly reducing their size.
 */
class LayerManager {
public:
    /**
     * @brief A singleton getter.
     * 
     * @return LayerManager* 
     */
    static LayerManager* get() noexcept;

    /// Returns a new Layer instance on success call; returns `nulltptr` otherwise.
    std::unique_ptr<style::Layer> createLayer(const std::string& type, const std::string& id,
                                              const style::conversion::Convertible& value, style::conversion::Error& error) noexcept;
    /// Returns a new RenderLayer instance on success call; returns `nulltptr` otherwise.
    std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept;

    /**
     * @brief a build-time flag to enable/disable annotations in mapbox-gl-native core.
     * 
     * At the moment, the annotations implementation in core is creating concrete
     * layer instances apart from LayerManager/LayerFactory code path.
     * 
     * So, annotations must be disabled if the LayerManager implementation does
     * not provide line, fill or symbol layers (those, used by the annotations 
     * implementation).
     * 
     * Note: in future, annotations implemantation will be moved from the core to platform 
     * SDK (see https://github.com/mapbox/mapbox-plugins-android/tree/master/plugin-annotation)
     * and this flag won't be needed any more.
     */
    static const bool annotationsEnabled;

protected:
    virtual ~LayerManager() = default;
    virtual LayerFactory* getFactory(const std::string& type) noexcept = 0;
    virtual LayerFactory* getFactory(const style::LayerTypeInfo*) noexcept = 0;
};

} // namespace mbgl
