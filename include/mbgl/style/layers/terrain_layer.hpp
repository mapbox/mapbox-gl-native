// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TerrainLayer : public Layer {
public:
    TerrainLayer(const std::string& layerID, const std::string& sourceID);
    ~TerrainLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    static PropertyValue<Color> getDefaultTerrainShadowColor();
    PropertyValue<Color> getTerrainShadowColor(const optional<std::string>& klass = {}) const;
    void setTerrainShadowColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultTerrainHighlightColor();
    PropertyValue<Color> getTerrainHighlightColor(const optional<std::string>& klass = {}) const;
    void setTerrainHighlightColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultTerrainAccentColor();
    PropertyValue<Color> getTerrainAccentColor(const optional<std::string>& klass = {}) const;
    void setTerrainAccentColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultTerrainIlluminationDirection();
    PropertyValue<float> getTerrainIlluminationDirection(const optional<std::string>& klass = {}) const;
    void setTerrainIlluminationDirection(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<AlignmentType> getDefaultTerrainIlluminationAlignment();
    PropertyValue<AlignmentType> getTerrainIlluminationAlignment(const optional<std::string>& klass = {}) const;
    void setTerrainIlluminationAlignment(PropertyValue<AlignmentType>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultTerrainExaggeration();
    PropertyValue<float> getTerrainExaggeration(const optional<std::string>& klass = {}) const;
    void setTerrainExaggeration(PropertyValue<float>, const optional<std::string>& klass = {});

    // Private implementation

    class Impl;
    Impl* const impl;

    TerrainLayer(const Impl&);
    TerrainLayer(const TerrainLayer&) = delete;
};

template <>
inline bool Layer::is<TerrainLayer>() const {
    return type == Type::Terrain;
}

} // namespace style
} // namespace mbgl
