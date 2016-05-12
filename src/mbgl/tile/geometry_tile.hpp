#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/constants.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace mbgl {

enum class FeatureType : uint8_t {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

// Normalized vector tile coordinates.
// Each geometry coordinate represents a point in a bidimensional space,
// varying from -V...0...+V, where V is the maximum extent applicable.
using GeometryCoordinate = Point<int16_t>;

class GeometryCoordinates : public std::vector<GeometryCoordinate> {
public:
    using coordinate_type = int16_t;
    using std::vector<GeometryCoordinate>::vector;
};

class GeometryCollection : public std::vector<GeometryCoordinates> {
public:
    using coordinate_type = int16_t;
    using std::vector<GeometryCoordinates>::vector;
};

class GeometryTileFeature : private util::noncopyable {
public:
    virtual ~GeometryTileFeature() = default;
    virtual FeatureType getType() const = 0;
    virtual optional<Value> getValue(const std::string& key) const = 0;
    virtual Feature::property_map getProperties() const { return Feature::property_map(); }
    virtual optional<uint64_t> getID() const { return {}; }
    virtual GeometryCollection getGeometries() const = 0;
};

class GeometryTileLayer : private util::noncopyable {
public:
    virtual ~GeometryTileLayer() = default;
    virtual std::size_t featureCount() const = 0;
    virtual util::ptr<const GeometryTileFeature> getFeature(std::size_t) const = 0;
    virtual std::string getName() const = 0;
};

class GeometryTile : private util::noncopyable {
public:
    virtual ~GeometryTile() = default;
    virtual util::ptr<GeometryTileLayer> getLayer(const std::string&) const = 0;
};

class AsyncRequest;

class GeometryTileMonitor : private util::noncopyable {
public:
    virtual ~GeometryTileMonitor() = default;

    using Callback = std::function<void (std::exception_ptr,
                                         std::unique_ptr<GeometryTile>,
                                         optional<Timestamp> modified,
                                         optional<Timestamp> expires)>;
    /*
     * Monitor the tile held by this object for changes. When the tile is loaded for the first time,
     * or updates, the callback is executed. If an error occurs, the first parameter will be set.
     * Otherwise it will be null. If there is no data for the requested tile, the second parameter
     * will be null.
     *
     * To cease monitoring, release the returned Request.
     */
    virtual std::unique_ptr<AsyncRequest> monitorTile(const Callback&) = 0;
};

} // namespace mbgl

#endif
