#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/util/pbf.hpp>

namespace mbgl {

class VectorTileLayer;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(pbf, const VectorTileLayer&);

    virtual FeatureType getType() const { return type; }
    virtual mapbox::util::optional<Value> getValue(const std::string&) const;
    virtual GeometryCollection getGeometries() const;

private:
    uint64_t id = 0;
    FeatureType type = FeatureType::Unknown;
    std::map<std::string, Value> properties;
    pbf geometry_pbf;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(pbf);

    virtual std::size_t featureCount() const { return features.size(); }
    virtual util::ptr<const GeometryTileFeature> feature(std::size_t) const;

private:
    friend class VectorTile;
    friend class VectorTileFeature;

    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::vector<Value> values;
    std::vector<pbf> features;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(pbf);

    virtual util::ptr<const GeometryTileLayer> getLayer(const std::string&) const;

private:
    std::map<std::string, util::ptr<const GeometryTileLayer>> layers;
};

}

#endif
