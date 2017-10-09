#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class AnnotationManager;
class TileParameters;

class AnnotationTile : public GeometryTile {
public:
    AnnotationTile(const OverscaledTileID&, const TileParameters&);
    ~AnnotationTile() override;

private:
    AnnotationManager& annotationManager;
};

class AnnotationTileFeatureData;

class AnnotationTileFeature : public GeometryTileFeature {
public:
    AnnotationTileFeature(std::shared_ptr<const AnnotationTileFeatureData>);
    ~AnnotationTileFeature() override;

    FeatureType getType() const override;
    optional<Value> getValue(const std::string&) const override;
    optional<FeatureIdentifier> getID() const override;
    GeometryCollection getGeometries() const override;

private:
    std::shared_ptr<const AnnotationTileFeatureData> data;
};

class AnnotationTileLayerData;

class AnnotationTileLayer : public GeometryTileLayer {
public:
    AnnotationTileLayer(std::shared_ptr<AnnotationTileLayerData>);

    std::size_t featureCount() const override;
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override;
    std::string getName() const override;

    void addFeature(const AnnotationID,
                    FeatureType,
                    GeometryCollection,
                    std::unordered_map<std::string, std::string> properties = { {} });

private:
    std::shared_ptr<AnnotationTileLayerData> layer;
};

class AnnotationTileData : public GeometryTileData {
public:
    std::unique_ptr<GeometryTileData> clone() const override;
    std::unique_ptr<GeometryTileLayer> getLayer(const std::string&) const override;

    std::unique_ptr<AnnotationTileLayer> addLayer(const std::string&);

private:
    std::unordered_map<std::string, std::shared_ptr<AnnotationTileLayerData>> layers;
};

} // namespace mbgl
