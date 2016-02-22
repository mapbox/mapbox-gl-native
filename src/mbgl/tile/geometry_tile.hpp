#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mapbox/variant.hpp>

#include <mbgl/style/value.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>
#include <string>
#include <vector>
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
using GeometryCoordinate  = vec2<int16_t>;
using GeometryCoordinates = std::vector<GeometryCoordinate>;
using GeometryCollection  = std::vector<GeometryCoordinates>;

class GeometryTileFeature : private util::noncopyable {
public:
    static const uint32_t defaultExtent = 4096;

    virtual ~GeometryTileFeature() = default;
    virtual FeatureType getType() const = 0;
    virtual optional<Value> getValue(const std::string& key) const = 0;
    virtual GeometryCollection getGeometries() const = 0;
    virtual uint32_t getExtent() const { return defaultExtent; }
};

class GeometryTileLayer : private util::noncopyable {
public:
    virtual ~GeometryTileLayer() = default;
    virtual std::size_t featureCount() const = 0;
    virtual util::ptr<const GeometryTileFeature> getFeature(std::size_t) const = 0;
};

class GeometryTile : private util::noncopyable {
public:
    virtual ~GeometryTile() = default;
    virtual util::ptr<GeometryTileLayer> getLayer(const std::string&) const = 0;
};

class FileRequest;

class GeometryTileMonitor : private util::noncopyable {
public:
    virtual ~GeometryTileMonitor() = default;

    using Callback = std::function<void (std::exception_ptr,
                                         std::unique_ptr<GeometryTile>,
                                         optional<SystemTimePoint> modified,
                                         optional<SystemTimePoint> expires)>;
    /*
     * Monitor the tile held by this object for changes. When the tile is loaded for the first time,
     * or updates, the callback is executed. If an error occurs, the first parameter will be set.
     * Otherwise it will be null. If there is no data for the requested tile, the second parameter
     * will be null.
     *
     * To cease monitoring, release the returned Request.
     */
    virtual std::unique_ptr<FileRequest> monitorTile(const Callback&) = 0;
};

class GeometryTileFeatureExtractor {
public:
    GeometryTileFeatureExtractor(const GeometryTileFeature& feature_)
        : feature(feature_) {}

    optional<Value> getValue(const std::string& key) const;

private:
    const GeometryTileFeature& feature;
};

} // namespace mbgl

#endif
