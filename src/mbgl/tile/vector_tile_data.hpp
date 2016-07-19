#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

#include <protozero/pbf_reader.hpp>

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>

namespace mbgl {

class VectorTileLayer;

namespace {

using pbf_iter_type = protozero::pbf_reader::const_uint32_iterator;
using packed_iter_type = std::pair<pbf_iter_type, pbf_iter_type>;

} // namespace

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(protozero::pbf_reader, const VectorTileLayer&);

    FeatureType getType() const override {
        return type;
    }
    optional<Value> getValue(const std::string&) const override;
    std::unordered_map<std::string, Value> getProperties() const override;
    optional<FeatureIdentifier> getID() const override;
    GeometryCollection getGeometries() const override;

private:
    const VectorTileLayer& layer;
    optional<FeatureIdentifier> id;
    FeatureType type = FeatureType::Unknown;
    packed_iter_type tags_iter;
    packed_iter_type geometry_iter;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(protozero::pbf_reader);

    std::size_t featureCount() const override {
        return features.size();
    }
    util::ptr<const GeometryTileFeature> getFeature(std::size_t) const override;
    std::string getName() const override;

private:
    friend class VectorTileData;
    friend class VectorTileFeature;

    std::string name;
    uint32_t version = 1;
    uint32_t extent = 4096;
    std::map<std::string, uint32_t> keysMap;
    std::vector<std::reference_wrapper<const std::string>> keys;
    std::vector<Value> values;
    std::vector<protozero::pbf_reader> features;
};

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(std::shared_ptr<const std::string> data);

    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;

private:
    std::shared_ptr<const std::string> data;
    mutable bool parsed = false;
    mutable std::map<std::string, util::ptr<GeometryTileLayer>> layers;
};

} // namespace mbgl
