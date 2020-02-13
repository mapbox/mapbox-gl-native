#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {

class StubGeometryTileFeature : public GeometryTileFeature {
public:
    StubGeometryTileFeature(PropertyMap properties_)
        : properties(std::move(properties_)) {
    }

    StubGeometryTileFeature(FeatureIdentifier id_, FeatureType type_, GeometryCollection geometry_, PropertyMap properties_)
        : properties(std::move(properties_)),
          id(std::move(id_)),
          type(type_),
          geometry(std::move(geometry_)) {
    }

    StubGeometryTileFeature(FeatureType type_, GeometryCollection geometry_)
        : type(type_), geometry(std::move(geometry_)) {}

    PropertyMap properties;
    FeatureIdentifier id;
    FeatureType type = FeatureType::Point;
    GeometryCollection geometry;

    FeatureType getType() const override {
        return type;
    }

    FeatureIdentifier getID() const override {
        return id;
    }

    optional<Value> getValue(const std::string& key) const override {
        return properties.count(key) ? properties.at(key) : optional<Value>();
    }

    const GeometryCollection& getGeometries() const override {
        return geometry;
    }
};

} // namespace mbgl
