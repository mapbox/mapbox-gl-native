#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class AnnotationManager;

namespace style {
class UpdateParameters;
} // namespace style

class AnnotationTile : public GeometryTile {
public:
    AnnotationTile(const OverscaledTileID&, const style::UpdateParameters&);
    ~AnnotationTile() override;

    void setNecessity(Necessity) final;

private:
    AnnotationManager& annotationManager;
};

class AnnotationTileFeature : public GeometryTileFeature {
public:
    AnnotationTileFeature(AnnotationID,
                          FeatureType,
                          GeometryCollection,
                          std::unordered_map<std::string, std::string> properties = {{}});

    FeatureType getType() const override {
        return type;
    }
    optional<Value> getValue(const std::string&) const override;
    optional<FeatureIdentifier> getID() const override {
        return {id};
    }
    GeometryCollection getGeometries() const override {
        return geometries;
    }

    const AnnotationID id;
    const FeatureType type;
    const std::unordered_map<std::string, std::string> properties;
    const GeometryCollection geometries;
};

class AnnotationTileLayer : public GeometryTileLayer {
public:
    AnnotationTileLayer(std::string);

    std::size_t featureCount() const override {
        return features.size();
    }
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override {
        return std::make_unique<AnnotationTileFeature>(features[i]);
    }
    std::string getName() const override {
        return name;
    };

    std::vector<AnnotationTileFeature> features;

private:
    std::string name;
};

class AnnotationTileData : public GeometryTileData {
public:
    std::unique_ptr<GeometryTileData> clone() const override;
    const GeometryTileLayer* getLayer(const std::string&) const override;

    std::unordered_map<std::string, AnnotationTileLayer> layers;
};

} // namespace mbgl
