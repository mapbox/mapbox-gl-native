#ifndef MBGL_MAP_GEOMETRY_TILE
#define MBGL_MAP_GEOMETRY_TILE

#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/value.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace mbgl {

class GeometryTileLayer;

enum class GeometryFeatureType {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

std::ostream& operator<<(std::ostream&, const GeometryFeatureType& type);

template <typename T>
class GeometryTileFeature {
public:
    uint64_t id = 0;
    GeometryFeatureType type = GeometryFeatureType::Unknown;
    std::map<std::string, Value> properties;
    T geometries;
};

template <typename T>
std::ostream& operator<<(std::ostream&, const GeometryTileFeature<T>& feature);

template <typename T>
class GeometryTileTagExtractor {
public:
    GeometryTileTagExtractor(const GeometryTileLayer&);

    void setTags(const T&);
    virtual mapbox::util::optional<Value> getValue(const std::string &key) const;
    inline void setType(GeometryFeatureType type_) { type = type_; }
    GeometryFeatureType getType() const { return type; }

protected:
    const GeometryTileLayer &layer;
    GeometryFeatureType type = GeometryFeatureType::Unknown;
};


template <typename T>
class GeometryFilteredTileLayer {
public:
    class iterator {
    public:
        iterator(const GeometryFilteredTileLayer&);
        virtual void operator++();
        virtual bool operator!=(const iterator& other) const;
        virtual const T& operator*() const;

    protected:
        const GeometryFilteredTileLayer& parent;
        bool valid = false;
    };

public:
    GeometryFilteredTileLayer(const GeometryTileLayer&, const FilterExpression&);

    iterator begin() const;
    iterator end() const;

public:
    const GeometryTileLayer& layer;
    const FilterExpression& filterExpression;
};

std::ostream& operator<<(std::ostream&, const PositionedGlyph&);

class GeometryTileLayer {
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
