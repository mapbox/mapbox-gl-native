#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/util/pbf.hpp>

#include <unordered_map>

namespace mbgl {

class VectorTileLayer;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(pbf, const VectorTileLayer&);

    FeatureType getType() const override { return type; }
    mapbox::util::optional<Value> getValue(const std::string&) const override;
    GeometryCollection getGeometries() const override;

private:
    const VectorTileLayer& layer;
    uint64_t id = 0;
    FeatureType type = FeatureType::Unknown;
    std::unordered_map<uint32_t, uint32_t> properties;
    pbf geometry_pbf;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(pbf);

    std::size_t featureCount() const override { return features.size(); }
    util::ptr<const GeometryTileFeature> feature(std::size_t) const override;

private:
    friend class VectorTile;
    friend class VectorTileFeature;

    std::string name;
    uint32_t extent = 4096;
    std::unordered_map<std::string, uint32_t> keys;
    std::vector<Value> values;
    std::vector<pbf> features;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(pbf);

    util::ptr<const GeometryTileLayer> getLayer(const std::string&) const override;

private:
    std::unordered_map<std::string, util::ptr<const GeometryTileLayer>> layers;
};

}

#endif
