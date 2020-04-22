#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>

#include <mapbox/std/weak.hpp>
#include <mapbox/util/type_wrapper.hpp>

#include <memory>
#include <string>

namespace mbgl {

class FileSource;

namespace style {

class VectorSource;
class RasterSource;
class RasterDEMSource;
class GeoJSONSource;
class SourceObserver;
struct LayerTypeInfo;

/**
 * The runtime representation of a [source](https://www.mapbox.com/mapbox-gl-style-spec/#sources) from the Mapbox Style
 * Specification.
 *
 * `Source` is an abstract base class; concrete derived classes are provided for each source type. `Source` contains
 * functionality that is common to all layer types:
 *
 * * Runtime type information: type predicates and casting
 * * Accessors for properties common to all source types: ID, etc.
 * * Cloning and copying
 *
 * All other functionality lives in the derived classes. To instantiate a source, create an instance of the desired
 * type, passing the ID:
 *
 *     auto vectorSource = std::make_unique<VectorSource>("my-vector-source");
 */
class Source {
public:
    Source(const Source&) = delete;
    Source& operator=(const Source&) = delete;

    virtual ~Source();

    // Check whether this source is of the given subtype.
    template <class T>
    bool is() const;

    // Dynamically cast this source to the given subtype.
    template <class T>
    T* as() {
        return is<T>() ? reinterpret_cast<T*>(this) : nullptr;
    }

    template <class T>
    const T* as() const {
        return is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
    }

    SourceType getType() const;
    std::string getID() const;
    optional<std::string> getAttribution() const;

    // The data from the volatile sources are not stored in a persistent storage.
    bool isVolatile() const noexcept;
    void setVolatile(bool) noexcept;

    // Private implementation
    class Impl;
    Immutable<Impl> baseImpl;

    void setObserver(SourceObserver*);
    SourceObserver* observer = nullptr;

    virtual void loadDescription(FileSource&) = 0;
    void setPrefetchZoomDelta(optional<uint8_t> delta) noexcept;
    optional<uint8_t> getPrefetchZoomDelta() const noexcept;

    // If the given source supports loading tiles from a server,
    // sets the minimum tile update interval.
    // Update network requests that are more frequent than the
    // minimum tile update interval are suppressed.
    //
    // Default value is `Duration::zero()`.
    void setMinimumTileUpdateInterval(Duration) noexcept;
    Duration getMinimumTileUpdateInterval() const noexcept;

    // Sets a limit for how much a parent tile can be overscaled.
    //
    // When a set of tiles for a current zoom level is being rendered and some of the
    // ideal tiles that cover the screen are not yet loaded, parent tile could be
    // used instead. This might introduce unwanted rendering side-effects, especially
    // for raster tiles that are overscaled multiple times.
    //
    // For example, an overscale factor of 3 would mean that on zoom level 3, the
    // minimum zoom level of a parent tile that could be used in place of an ideal
    // tile during rendering would be zoom 0. By default, no limit is set, so any
    // parent tile may be used.
    void setMaxOverscaleFactorForParentTiles(optional<uint8_t> overscaleFactor) noexcept;
    optional<uint8_t> getMaxOverscaleFactorForParentTiles() const noexcept;
    void dumpDebugLogs() const;

    virtual bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const = 0;

    bool loaded = false;

    // For use in SDK bindings, which store a reference to a platform-native peer
    // object here, so that separately-obtained references to this object share
    // identical platform-native peers.
    mapbox::base::TypeWrapper peer;

    virtual mapbox::base::WeakPtr<Source> makeWeakPtr() = 0;

protected:
    explicit Source(Immutable<Impl>);
    virtual Mutable<Impl> createMutable() const noexcept = 0;
};

} // namespace style
} // namespace mbgl
