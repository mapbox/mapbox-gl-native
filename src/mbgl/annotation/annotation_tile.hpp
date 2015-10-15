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
    mapbox::util::optional<Value> getValue(const std::string&) const override;
    GeometryCollection getGeometries() const override { return geometries; }

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

}

#endif
