// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

namespace mbgl {

class CircleLayer : public Layer {
public:
    CircleLayer(const std::string& layerID);
    ~CircleLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    PropertyValue<float> getCircleRadius() const;
    void setCircleRadius(PropertyValue<float>);

    PropertyValue<Color> getCircleColor() const;
    void setCircleColor(PropertyValue<Color>);

    PropertyValue<float> getCircleBlur() const;
    void setCircleBlur(PropertyValue<float>);

    PropertyValue<float> getCircleOpacity() const;
    void setCircleOpacity(PropertyValue<float>);

    PropertyValue<std::array<float, 2>> getCircleTranslate() const;
    void setCircleTranslate(PropertyValue<std::array<float, 2>>);

    PropertyValue<TranslateAnchorType> getCircleTranslateAnchor() const;
    void setCircleTranslateAnchor(PropertyValue<TranslateAnchorType>);

    // Private implementation

    class Impl;
    Impl* const impl;

    CircleLayer(const Impl&);
    CircleLayer(const CircleLayer&) = delete;
};

template <>
inline bool Layer::is<CircleLayer>() const {
    return type == Type::Circle;
}

} // namespace mbgl
