#include <mbgl/tile/geometry_tile_data.hpp>

#include <mbgl/util/blob.hpp>

#include <mapbox/vector_tile.hpp>
#include <protozero/pbf_reader.hpp>

#include <unordered_map>
#include <functional>
#include <utility>

namespace mbgl {

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(const mapbox::vector_tile::layer&, const protozero::data_view&);

    FeatureType getType() const override;
    optional<Value> getValue(const std::string& key) const override;
    std::unordered_map<std::string, Value> getProperties() const override;
    optional<FeatureIdentifier> getID() const override;
    GeometryCollection getGeometries() const override;

private:
    mapbox::vector_tile::feature feature;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(std::shared_ptr<const std::string> data, const protozero::data_view&);

    std::size_t featureCount() const override;
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override;
    std::string getName() const override;

private:
    std::shared_ptr<const std::string> data;
    mapbox::vector_tile::layer layer;
};

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(const VectorTileData&);
    VectorTileData(Blob blob);

    std::unique_ptr<GeometryTileData> clone() const override;
    std::unique_ptr<GeometryTileLayer> getLayer(const std::string& name) const override;

    std::vector<std::string> layerNames() const;

private:
    Blob blob;
    mutable std::shared_ptr<const std::string> data;
    mutable std::map<std::string, const protozero::data_view> layers;
};

} // namespace mbgl
