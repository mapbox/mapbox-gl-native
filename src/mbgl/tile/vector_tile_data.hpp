#include <mbgl/tile/geometry_tile_data.hpp>

#include <vtzero/vector_tile.hpp>
#include <protozero/pbf_reader.hpp>

#include <map>
#include <unordered_map>
#include <functional>
#include <utility>

namespace mbgl {

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(const vtzero::layer&, const vtzero::feature&);

    FeatureType getType() const override;
    optional<Value> getValue(const std::string& key) const override;
    std::unordered_map<std::string, Value> getProperties() const override;
    optional<FeatureIdentifier> getID() const override;
    GeometryCollection getGeometries() const override;

private:
    vtzero::feature feature;

    // hold this reference because feature depends on it
    const vtzero::layer& layer;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(std::shared_ptr<const std::string> data, const vtzero::layer&);

    std::size_t featureCount() const override;
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override;
    std::string getName() const override;

private:
    std::shared_ptr<const std::string> data;
    vtzero::layer layer;
    std::vector<vtzero::feature> features;
};

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(std::shared_ptr<const std::string> data);

    std::unique_ptr<GeometryTileData> clone() const override;
    std::unique_ptr<GeometryTileLayer> getLayer(const std::string& name) const override;

    std::vector<std::string> layerNames() const;

private:
    void parseLayers() const;
    
    std::shared_ptr<const std::string> data;
    vtzero::vector_tile tile;
    mutable bool parsed = false;
    mutable std::map<std::string, vtzero::layer> layers;
};

} // namespace mbgl
