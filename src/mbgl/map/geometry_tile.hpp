#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/value.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace mbgl {

enum class GeometryFeatureType : uint8_t {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

typedef Coordinate GeometryPoint;
typedef std::vector<GeometryPoint> GeometryLine;
typedef std::vector<GeometryLine> GeometryPolygon;

using Geometry = mapbox::util::variant<std::false_type, GeometryPoint, GeometryLine, GeometryPolygon>;

typedef std::vector<Geometry> GeometryCollection;

class GeometryTileFeature : public mbgl::util::noncopyable {
public:
    virtual GeometryFeatureType getType() const = 0;
    virtual mapbox::util::optional<Value> getValue(const std::string& key) const = 0;
    virtual GeometryCollection getGeometries() const = 0;
};

class GeometryTileLayer : public mbgl::util::noncopyable {
public:
    virtual std::size_t featureCount() const = 0;
    virtual const util::ptr<const GeometryTileFeature> feature(std::size_t i) const = 0;
};

class GeometryTile : public mbgl::util::noncopyable {
public:
    virtual const util::ptr<const GeometryTileLayer> getLayer(const std::string&) const = 0;
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
