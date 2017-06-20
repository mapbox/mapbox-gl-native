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

    void setNecessity(Necessity) final;

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

class AnnotationTileLayer : public GeometryTileLayer {
public:
    AnnotationTileLayer(std::string);

    std::size_t featureCount() const override {
        return features.size();
    }

    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override {
        return std::make_unique<AnnotationTileFeature>(features.at(i));
    }

    std::string getName() const override {
        return name;
    }

    void addFeature(const AnnotationID,
                    FeatureType,
                    GeometryCollection,
                    std::unordered_map<std::string, std::string> properties = { {} });

private:
    std::vector<std::shared_ptr<const AnnotationTileFeatureData>> features;
    std::string name;
};

class AnnotationTileData : public GeometryTileData {
public:
    std::unique_ptr<GeometryTileData> clone() const override;
    const GeometryTileLayer* getLayer(const std::string&) const override;
    AnnotationTileLayer& addLayer(const std::string&);

private:
    std::unordered_map<std::string, AnnotationTileLayer> layers;
};

} // namespace mbgl
