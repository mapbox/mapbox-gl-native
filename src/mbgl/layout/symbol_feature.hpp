#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>

#include <array>
#include <string>

namespace mbgl {

class SymbolFeature : public GeometryTileFeature {
public:
    SymbolFeature(std::unique_ptr<GeometryTileFeature> feature_) :
        feature(std::move(feature_)),
        geometry(feature->getGeometries()) // we need a mutable copy of the geometry for mergeLines()
    {}
    
    FeatureType getType() const override { return feature->getType(); }
    optional<Value> getValue(const std::string& key) const override { return feature->getValue(key); };
    std::unordered_map<std::string,Value> getProperties() const override { return feature->getProperties(); };
    optional<FeatureIdentifier> getID() const override { return feature->getID(); };
    GeometryCollection getGeometries() const override { return geometry; };

    std::unique_ptr<GeometryTileFeature> feature;
    GeometryCollection geometry;
    optional<std::u16string> text;
    optional<std::string> icon;
    std::size_t index;
};

} // namespace mbgl
