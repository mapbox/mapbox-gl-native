#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mapbox/variant.hpp>
#include <mapbox/optional.hpp>

#include <mbgl/style/value.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/noncopyable.hpp>

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

typedef std::vector<std::vector<Coordinate>> GeometryCollection;

class GeometryTileFeature : private util::noncopyable {
public:
    virtual FeatureType getType() const = 0;
    virtual mapbox::util::optional<Value> getValue(const std::string& key) const = 0;
    virtual GeometryCollection getGeometries() const = 0;
};

class GeometryTileLayer : private util::noncopyable {
public:
    virtual std::size_t featureCount() const = 0;
    virtual util::ptr<const GeometryTileFeature> getFeature(std::size_t) const = 0;
};

class GeometryTile : private util::noncopyable {
public:
    virtual util::ptr<GeometryTileLayer> getLayer(const std::string&) const = 0;
};

class Request;

class GeometryTileMonitor : private util::noncopyable {
public:
    /*
     * Monitor the tile held by this object for changes. When the tile is loaded for the first time,
     * or updates, the callback is executed. If an error occurs, the first parameter will be set.
     * Otherwise it will be null. If there is no data for the requested tile, the second parameter
     * will be null.
     *
     * To cease monitoring, release the returned Request.
     */
    virtual Request* monitorTile(std::function<void (std::exception_ptr, std::unique_ptr<GeometryTile>)>) = 0;
};

class GeometryTileFeatureExtractor {
public:
    GeometryTileFeatureExtractor(const GeometryTileFeature& feature_)
        : feature(feature_) {}

    mapbox::util::optional<Value> getValue(const std::string& key) const;

private:
    const GeometryTileFeature& feature;
};

}

#endif
