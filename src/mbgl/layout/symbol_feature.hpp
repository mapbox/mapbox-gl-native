#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/text/tagged_string.hpp>

#include <array>
#include <string>

namespace mbgl {

class SymbolFeature : public GeometryTileFeature {
public:
    SymbolFeature(std::unique_ptr<GeometryTileFeature> feature_) :
        feature(std::move(feature_)),
        geometry(feature->getGeometries().clone()) // we need a mutable copy of the geometry for mergeLines()
    {}
    
    FeatureType getType() const override { return feature->getType(); }
    optional<Value> getValue(const std::string& key) const override { return feature->getValue(key); };
    const PropertyMap& getProperties() const override { return feature->getProperties(); };
    FeatureIdentifier getID() const override { return feature->getID(); };
    const GeometryCollection& getGeometries() const override { return geometry; };

    friend bool operator < (const SymbolFeature& lhs, const SymbolFeature& rhs) {
        return lhs.sortKey <  rhs.sortKey;
    }

    std::unique_ptr<GeometryTileFeature> feature;
    GeometryCollection geometry;
    optional<TaggedString> formattedText;
    optional<std::string> icon;
    float sortKey = 0.0f;
    std::size_t index;
    bool allowsVerticalWritingMode = false;
};

} // namespace mbgl
