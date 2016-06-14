// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class FillLayer : public Layer {
public:
    FillLayer(const std::string& layerID);
    ~FillLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    PropertyValue<bool> getFillAntialias() const;
    void setFillAntialias(PropertyValue<bool>);

    PropertyValue<float> getFillOpacity() const;
    void setFillOpacity(PropertyValue<float>);

    PropertyValue<Color> getFillColor() const;
    void setFillColor(PropertyValue<Color>);

    PropertyValue<Color> getFillOutlineColor() const;
    void setFillOutlineColor(PropertyValue<Color>);

    PropertyValue<std::array<float, 2>> getFillTranslate() const;
    void setFillTranslate(PropertyValue<std::array<float, 2>>);

    PropertyValue<TranslateAnchorType> getFillTranslateAnchor() const;
    void setFillTranslateAnchor(PropertyValue<TranslateAnchorType>);

    PropertyValue<std::string> getFillPattern() const;
    void setFillPattern(PropertyValue<std::string>);

    // Private implementation

    class Impl;
    Impl* const impl;

    FillLayer(const Impl&);
    FillLayer(const FillLayer&) = delete;
};

template <>
inline bool Layer::is<FillLayer>() const {
    return type == Type::Fill;
}

} // namespace style
} // namespace mbgl
