#ifndef MBGL_ANNOTATION_TILE
#define MBGL_ANNOTATION_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/tile_id.hpp>

#include <map>
#include <unordered_map>

namespace mbgl {

class AnnotationTileFeature : public GeometryTileFeature {
public:
    AnnotationTileFeature(FeatureType, GeometryCollection,
                          std::unordered_map<std::string, std::string> properties = {{}});

    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string&) const override;
    GeometryCollection getGeometries() const override { return geometries; }
    uint32_t getExtent() const override { return 4096; }

    const FeatureType type;
    const std::unordered_map<std::string, std::string> properties;
    const GeometryCollection geometries;
};

class AnnotationTileLayer : public GeometryTileLayer {
public:
    std::size_t featureCount() const override { return features.size(); }
    util::ptr<const GeometryTileFeature> getFeature(std::size_t i) const override { return features[i]; }

    std::vector<util::ptr<const AnnotationTileFeature>> features;
};

class AnnotationTile : public GeometryTile {
public:
    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;

    std::map<std::string, util::ptr<AnnotationTileLayer>> layers;
};

class MapData;

class AnnotationTileMonitor : public GeometryTileMonitor {
public:
    // TODO: should just take AnnotationManager&, but we need to eliminate util::exclusive<AnnotationManager> from MapData first.
    AnnotationTileMonitor(const TileID&, MapData&);
    ~AnnotationTileMonitor();

    void update(std::unique_ptr<GeometryTile>);
    std::unique_ptr<FileRequest> monitorTile(const GeometryTileMonitor::Callback&) override;

    TileID tileID;

private:
    MapData& data;
    GeometryTileMonitor::Callback callback;
};

} // namespace mbgl

#endif
