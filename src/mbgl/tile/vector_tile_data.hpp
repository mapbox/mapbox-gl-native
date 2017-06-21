#include <mbgl/tile/geometry_tile_data.hpp>

#include <protozero/pbf_reader.hpp>

#include <unordered_map>
#include <functional>
#include <utility>

namespace mbgl {

class VectorTileLayer;

using packed_iter_type = protozero::iterator_range<protozero::pbf_reader::const_uint32_iterator>;

struct VectorTileLayerData {
    VectorTileLayerData(std::shared_ptr<const std::string>);

    // Hold a reference to the underlying pbf data that backs the lazily-built
    // components of the owning VectorTileLayer and VectorTileFeature objects
    std::shared_ptr<const std::string> data;

    uint32_t version = 1;
    uint32_t extent = 4096;
    std::unordered_map<std::string, uint32_t> keysMap;
    std::vector<std::reference_wrapper<const std::string>> keys;
    std::vector<Value> values;
};

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(protozero::pbf_reader, std::shared_ptr<VectorTileLayerData> layerData);

    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string&) const override;
    std::unordered_map<std::string,Value> getProperties() const override;
    optional<FeatureIdentifier> getID() const override;
    GeometryCollection getGeometries() const override;

private:
    std::shared_ptr<VectorTileLayerData> layerData;
    optional<FeatureIdentifier> id;
    FeatureType type = FeatureType::Unknown;
    packed_iter_type tags_iter;
    packed_iter_type geometry_iter;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(protozero::pbf_reader, std::shared_ptr<const std::string>);

    std::size_t featureCount() const override { return features.size(); }
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t) const override;
    std::string getName() const override;

private:
    friend class VectorTileData;
    friend class VectorTileFeature;

    std::string name;
    std::vector<protozero::pbf_reader> features;
    std::shared_ptr<VectorTileLayerData> data;
};

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(std::shared_ptr<const std::string> data);

    std::unique_ptr<GeometryTileData> clone() const override {
        return std::make_unique<VectorTileData>(*this);
    }

    const GeometryTileLayer* getLayer(const std::string&) const override;

    std::vector<std::string> layerNames() const;

private:
    void parse() const;

private:
    std::shared_ptr<const std::string> data;
    mutable bool parsed = false;
    mutable std::unordered_map<std::string, VectorTileLayer> layers;
};

} // namespace mbgl
