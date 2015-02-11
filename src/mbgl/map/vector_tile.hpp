#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/value.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace mbgl {

class VectorTileLayer;

enum class FeatureType {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

std::ostream& operator<<(std::ostream&, const FeatureType& type);

class VectorTileFeature {
public:
    VectorTileFeature(pbf feature, const VectorTileLayer& layer);

    uint64_t id = 0;
    FeatureType type = FeatureType::Unknown;
    std::map<std::string, Value> properties;
    pbf geometry;
};

std::ostream& operator<<(std::ostream&, const VectorTileFeature& feature);


class VectorTileTagExtractor {
public:
    VectorTileTagExtractor(const VectorTileLayer &layer);

    void setTags(const pbf &pbf);
    mapbox::util::optional<Value> getValue(const std::string &key) const;
    void setType(FeatureType type);
    FeatureType getType() const;

private:
    const VectorTileLayer &layer_;
    pbf tags_;
    FeatureType type_ = FeatureType::Unknown;
};

/*
 * Allows iterating over the features of a VectorTileLayer using a
 * BucketDescription as filter. Only features matching the descriptions will
 * be returned (as pbf).
 */
class FilteredVectorTileLayer {
public:
    class iterator {
    public:
        iterator(const FilteredVectorTileLayer& filter, const pbf& data);
        void operator++();
        bool operator!=(const iterator& other) const;
        const pbf& operator*() const;

    private:
        const FilteredVectorTileLayer& parent;
        bool valid = false;
        pbf feature;
        pbf data;
    };

public:
    FilteredVectorTileLayer(const VectorTileLayer& layer, const FilterExpression &filterExpression);

    iterator begin() const;
    iterator end() const;

private:
    const VectorTileLayer& layer;
    const FilterExpression& filterExpression;
};

std::ostream& operator<<(std::ostream&, const PositionedGlyph& placement);

class VectorTileLayer {
public:
    VectorTileLayer(pbf data);

    const pbf data;
    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::unordered_map<std::string, uint32_t> key_index;
    std::vector<Value> values;
    std::map<std::string, std::map<Value, Shaping>> shaping;
};

class VectorTile {
public:
    VectorTile();
    VectorTile(pbf data);
    VectorTile& operator=(VectorTile&& other);

    std::map<std::string, const VectorTileLayer> layers;
};



}

#endif
