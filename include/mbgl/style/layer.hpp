#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/style_property.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>

#include <mapbox/weak.hpp>
#include <mapbox/type_wrapper.hpp>

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
     * @brief contains \c FadingTiles::Required if the corresponding layer type
     * requires rendering on fading tiles. Contains \c FadingTiles::NotRequired otherwise.
     */
    const enum class FadingTiles { Required, NotRequired } fadingTiles;

    /**
     * @brief contains \c CrossTileIndex::Required if the corresponding layer type
     * requires cross-tile indexing and placement. Contains \c CrossTileIndex::NotRequired otherwise.
     */
    const enum class CrossTileIndex { Required, NotRequired } crossTileIndex;

    /**
     * @brief contains the Id of the supported tile type. Used for internal checks.
     * The contained values correspond to \c Tile::Kind enum.
     */
    const enum class TileKind : uint8_t { Geometry, Raster, RasterDEM, NotRequired } tileKind;
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
    virtual optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) = 0;
    optional<conversion::Error> setVisibility(const conversion::Convertible& value);

    virtual StyleProperty getProperty(const std::string&) const = 0;

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
    mapbox::base::TypeWrapper peer;
    Layer(Immutable<Impl>);

    const LayerTypeInfo* getTypeInfo() const noexcept;

    mapbox::base::WeakPtr<Layer> makeWeakPtr() {
        return weakFactory.makeWeakPtr();
    }

protected:
    virtual Mutable<Impl> mutableBaseImpl() const = 0;

    LayerObserver* observer;
    mapbox::base::WeakPtrFactory<Layer> weakFactory {this};
};

} // namespace style
} // namespace mbgl
