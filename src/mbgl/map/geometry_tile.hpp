#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/value.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/ptr.hpp>
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

std::ostream& operator<<(std::ostream&, const GeometryFeatureType& type);

class GeometryTileLayer;

class GeometryTileFeature {
public:
    virtual GeometryCollection nextGeometry() = 0;

public:
    uint64_t id = 0;
    GeometryFeatureType type = GeometryFeatureType::Unknown;
    std::map<std::string, Value> properties;
};

std::ostream& operator<<(std::ostream&, const GeometryTileFeature& feature);

template <typename Tags>
class GeometryTileTagExtractor {
public:
    GeometryTileTagExtractor(const util::ptr<GeometryTileLayer>);

    inline void setTags(const Tags& tags_) { tags = tags_; }
    virtual mapbox::util::optional<Value> getValue(const std::string &key) const;

    inline void setType(GeometryFeatureType type_) { type = type_; }
    inline GeometryFeatureType getType() const { return type; }

public:
    GeometryFeatureType type = GeometryFeatureType::Unknown;
    Tags tags;

protected:
    const util::ptr<GeometryTileLayer> layer;
};

class GeometryFilteredTileLayer {
public:
    GeometryFilteredTileLayer(const util::ptr<GeometryTileLayer>, const FilterExpression&);

    virtual util::ptr<GeometryTileFeature> nextMatchingFeature() = 0;

protected:
    const util::ptr<GeometryTileLayer> layer;
    const FilterExpression& filterExpression;
};

std::ostream& operator<<(std::ostream&, const PositionedGlyph&);

class GeometryTileLayer {
public:
    virtual util::ptr<GeometryFilteredTileLayer> createFilter(const FilterExpression&) = 0;
    virtual util::ptr<GeometryTileFeature> nextFeature() = 0;

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

    virtual void logDebug() const = 0;

public:
    std::map<std::string, const util::ptr<GeometryTileLayer>> layers;
};

}

#endif
