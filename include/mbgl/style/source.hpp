#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/unique_any.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/style/types.hpp>

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
class Source : public mbgl::util::noncopyable {
public:
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

    // Private implementation
    class Impl;
    Immutable<Impl> baseImpl;

    Source(Immutable<Impl>);

    void setObserver(SourceObserver*);
    SourceObserver* observer = nullptr;

    virtual void loadDescription(FileSource&) = 0;
    void dumpDebugLogs() const;

    bool loaded = false;

    // For use in SDK bindings, which store a reference to a platform-native peer
    // object here, so that separately-obtained references to this object share
    // identical platform-native peers.
    util::unique_any peer;
};

} // namespace style
} // namespace mbgl
