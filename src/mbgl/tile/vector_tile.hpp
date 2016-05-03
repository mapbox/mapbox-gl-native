#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/map/tile_id.hpp>
#include <protozero/pbf_reader.hpp>

#include <map>
#include <unordered_map>
#include <functional>

namespace mbgl {

class VectorTileLayer;

using pbf_iter_type = protozero::pbf_reader::const_uint32_iterator;
using packed_iter_type = std::pair<pbf_iter_type,pbf_iter_type>;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(protozero::pbf_reader, const VectorTileLayer&);

    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string&) const override;
    std::unordered_map<std::string,Value> getProperties() const override;
    uint64_t getID() const override;
    GeometryCollection getGeometries() const override;
    uint32_t getExtent() const override;

private:
    const VectorTileLayer& layer;
    uint64_t id = 0;
    FeatureType type = FeatureType::Unknown;
    packed_iter_type tags_iter;
    packed_iter_type geometry_iter;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(protozero::pbf_reader);

    std::size_t featureCount() const override { return features.size(); }
    util::ptr<const GeometryTileFeature> getFeature(std::size_t) const override;
    std::string getName() const override;

private:
    friend class VectorTile;
    friend class VectorTileFeature;

    std::string name;
    uint32_t extent = 4096;
    std::map<std::string, uint32_t> keysMap;
    std::vector<std::reference_wrapper<const std::string>> keys;
    std::vector<Value> values;
    std::vector<protozero::pbf_reader> features;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(std::shared_ptr<const std::string> data);

    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;

private:
    std::shared_ptr<const std::string> data;
    mutable bool parsed = false;
    mutable std::map<std::string, util::ptr<GeometryTileLayer>> layers;
};

class TileID;
class FileSource;

class VectorTileMonitor : public GeometryTileMonitor {
public:
    VectorTileMonitor(const TileID&, float pixelRatio, const std::string& urlTemplate, FileSource&);

    std::unique_ptr<AsyncRequest> monitorTile(const GeometryTileMonitor::Callback&) override;

private:
    TileID tileID;
    float pixelRatio;
    std::string urlTemplate;
    FileSource& fileSource;
};

} // namespace mbgl

#endif
