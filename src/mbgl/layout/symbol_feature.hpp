#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>

#include <array>
#include <string>

namespace mbgl {

class SymbolFeature : public GeometryTileFeature {
public:
    SymbolFeature(const std::unique_ptr<GeometryTileFeature>& feature) :
        id(feature->getID()),
        type(feature->getType()),
        geometry(feature->getGeometries()),
        properties(feature->getProperties())
    {}

    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string& key) const override {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return it->second;
        }
        return {};
    };
    std::unordered_map<std::string,Value> getProperties() const override { return properties; };
    optional<FeatureIdentifier> getID() const override { return id; };
    GeometryCollection getGeometries() const override { return geometry; };

    optional<FeatureIdentifier> id;
    FeatureType type;
    GeometryCollection geometry;
    std::unordered_map<std::string,Value> properties;
    
    optional<std::u16string> text;
    optional<std::string> icon;
    std::array<float, 2> iconOffset;
    float iconRotation;
    std::size_t index;
};

} // namespace mbgl
