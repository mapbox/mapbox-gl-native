#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/filter_expression.hpp>

namespace mbgl {

class VectorTileLayer;
struct pbf;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(pbf, const GeometryTileLayer&);

    virtual inline uint64_t getID() const { return id; };
    virtual inline GeometryFeatureType getType() const { return type; }
    virtual inline std::map<std::string, Value> getProperties() const { return properties; }
    virtual GeometryCollection nextGeometry();

private:
    uint64_t id = 0;
    GeometryFeatureType type = GeometryFeatureType::Unknown;
    std::map<std::string, Value> properties;
    pbf geometry_pbf;
};

class FilteredVectorTileLayer : public GeometryFilteredTileLayer {
public:
    class iterator : public GeometryFilteredTileLayer::iterator {
    public:
        iterator(const FilteredVectorTileLayer&, const pbf&);
        void operator++();
        bool operator!=(const iterator&) const;
        const VectorTileFeature& operator*() const;

    private:
        const FilteredVectorTileLayer& parent;
        bool valid = false;
        VectorTileFeature feature;
        pbf feature_pbf;
    };

public:
    FilteredVectorTileLayer(const VectorTileLayer&, const FilterExpression&);

    virtual GeometryFilteredTileLayer::iterator begin() const;
    virtual GeometryFilteredTileLayer::iterator end() const;

private:
    const VectorTileLayer& layer;
    const FilterExpression& filterExpression;
};

class VectorTileLayer : public GeometryTileLayer {
    friend class FilteredVectorTileLayer;
public:
    VectorTileLayer(pbf);

    virtual inline const std::string getName() const { return name; }
    virtual inline uint32_t getExtent() const { return extent; }
    virtual inline const std::vector<std::string> getKeys() const { return keys; }
    virtual inline const std::unordered_map<std::string, uint32_t> getKeyIndex() const { return key_index; }
    virtual inline const std::vector<Value> getValues() const { return values; }
    virtual inline const std::map<std::string, std::map<Value, Shaping>> getShaping() const { return shaping; }

    virtual std::unique_ptr<GeometryFilteredTileLayer> createFilteredTileLayer(const FilterExpression&) const;

private:
    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::unordered_map<std::string, uint32_t> key_index;
    std::vector<Value> values;
    std::map<std::string, std::map<Value, Shaping>> shaping;
    const pbf feature_pbf;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(pbf);

    VectorTile& operator=(VectorTile&&);
    virtual std::map<std::string, const GeometryTileLayer> getLayers() const;

private:
    std::map<std::string, const VectorTileLayer> layers;
};

}

#endif
