#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/value.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/vec.hpp>

#include <cstdint>
#include <iosfwd>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace mbgl {

enum class GeometryFeatureType {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

typedef Coordinate GeometryPoint;
typedef std::vector<GeometryPoint> GeometryLine;
typedef std::vector<GeometryLine> GeometryPolygon;

using Geometry = mapbox::util::variant<std::false_type, GeometryPoint, GeometryLine, GeometryPolygon>;

std::ostream& operator<<(std::ostream&, const GeometryFeatureType& type);

class GeometryTileLayer;

class GeometryTileFeature {
public:
    GeometryTileFeature();

    virtual Geometry nextGeometry();

public:
    uint64_t id = 0;
    GeometryFeatureType type = GeometryFeatureType::Unknown;
    std::map<std::string, Value> properties;
};

std::ostream& operator<<(std::ostream&, const GeometryTileFeature& feature);

template <typename T>
class GeometryTileTagExtractor {
public:
    GeometryTileTagExtractor(const GeometryTileLayer&);

    inline void setTags(const T& tags_) { tags = tags_; }
    mapbox::util::optional<Value> getValue(const std::string &key) const;

    inline void setType(GeometryFeatureType type_) { type = type_; }
    inline GeometryFeatureType getType() const { return type; }

protected:
    const GeometryTileLayer& layer;
    T tags;
    GeometryFeatureType type = GeometryFeatureType::Unknown;
};

class GeometryFilteredTileLayer {
public:
    GeometryFilteredTileLayer(const GeometryTileLayer&, const FilterExpression&);

    virtual GeometryTileFeature nextMatchingFeature();

protected:
    const GeometryTileLayer& layer;
    const FilterExpression& filterExpression;
};

std::ostream& operator<<(std::ostream&, const PositionedGlyph&);

class GeometryTileLayer {
public:
    virtual GeometryTileFeature nextFeature();

public:
    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::unordered_map<std::string, uint32_t> key_index;
    std::vector<Value> values;
    std::map<std::string, std::map<Value, Shaping>> shaping;
};

class GeometryTile {
public:
    GeometryTile& operator=(GeometryTile&& other);

public:
    std::map<std::string, const GeometryTileLayer> layers;
};

}

#endif
