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
    virtual uint64_t getID() const = 0;
    virtual GeometryFeatureType getType() const = 0;
    virtual std::map<std::string, Value> getProperties() const = 0;
    virtual GeometryCollection nextGeometry() = 0;
};

std::ostream& operator<<(std::ostream&, const GeometryTileFeature& feature);

template <typename Tags>
class GeometryTileTagExtractor {
public:
    GeometryTileTagExtractor(const GeometryTileLayer&);

    inline void setTags(const Tags& tags_) { tags = tags_; }
    virtual mapbox::util::optional<Value> getValue(const std::string &key) const;

    inline void setType(GeometryFeatureType type_) { type = type_; }
    inline GeometryFeatureType getType() const { return type; }

public:
    GeometryFeatureType type = GeometryFeatureType::Unknown;
    Tags tags;

protected:
    const GeometryTileLayer& layer;
};

class GeometryFilteredTileLayer {
public:
    class iterator {
    public:
        void operator++();
        bool operator!=(const iterator&) const;
        GeometryTileFeature& operator*() const;
    };

public:
    virtual iterator begin() const = 0;
    virtual iterator end() const = 0;
};

std::ostream& operator<<(std::ostream&, const PositionedGlyph&);

class GeometryTileLayer {
public:
    virtual const std::string& getName() const = 0;
    virtual uint32_t getExtent() const = 0;
    virtual const std::vector<std::string>& getKeys() const = 0;
    virtual const std::unordered_map<std::string, uint32_t>& getKeyIndex() const = 0;
    virtual const std::vector<Value>& getValues() const = 0;
    virtual const std::map<std::string, std::map<Value, Shaping>>& getShaping() const = 0;
    virtual std::unique_ptr<GeometryFilteredTileLayer> createFilteredTileLayer(const FilterExpression&) const = 0;
};

class GeometryTile {
public:
    virtual const util::ptr<const GeometryTileLayer> getLayer(const std::string&) const = 0;
};

}

#endif
