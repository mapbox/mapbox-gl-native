#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class AnnotationTileData;
class TileParameters;

class AnnotationTile : public GeometryTile {
public:
    AnnotationTile(const OverscaledTileID&,
                   const TileParameters&,
                   std::unique_ptr<const AnnotationTileData> data);
    ~AnnotationTile() override;

    void setNecessity(Necessity) final;
};

class AnnotationTileFeature : public GeometryTileFeature {
public:
    AnnotationTileFeature(AnnotationID, FeatureType, GeometryCollection,
                          std::unordered_map<std::string, std::string> properties = {{}});

    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string&) const override;
    optional<FeatureIdentifier> getID() const override { return { static_cast<uint64_t>(id) }; }
    GeometryCollection getGeometries() const override { return geometries; }

    const AnnotationID id;
    const FeatureType type;
    const std::unordered_map<std::string, std::string> properties;
    const GeometryCollection geometries;
};

class AnnotationTileLayer : public GeometryTileLayer {
public:
    AnnotationTileLayer(std::string);

    std::size_t featureCount() const override { return features.size(); }

    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override {
        return std::make_unique<AnnotationTileFeature>(features.at(i));
    }

    std::string getName() const override { return name; };

    std::vector<AnnotationTileFeature> features;

private:
    std::string name;
};

/**
 * Holds the layers for a AnnotationTile. One layer per shape geometry
 * and a single layer for all symbol geometries
 */
class AnnotationTileData : public GeometryTileData {
public:
    std::unique_ptr<GeometryTileData> clone() const override;
    const GeometryTileLayer* getLayer(const std::string&) const override;

    std::unordered_map<std::string, AnnotationTileLayer> layers;
};

} // namespace mbgl
