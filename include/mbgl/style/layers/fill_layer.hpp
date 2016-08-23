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
    FillLayer(const std::string& layerID, const std::string& sourceID);
    ~FillLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    static PropertyValue<bool> getDefaultFillAntialias();
    PropertyValue<bool> getFillAntialias() const;
    void setFillAntialias(PropertyValue<bool>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultFillOpacity();
    PropertyValue<float> getFillOpacity() const;
    void setFillOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultFillColor();
    PropertyValue<Color> getFillColor() const;
    void setFillColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultFillOutlineColor();
    PropertyValue<Color> getFillOutlineColor() const;
    void setFillOutlineColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<std::array<float, 2>> getDefaultFillTranslate();
    PropertyValue<std::array<float, 2>> getFillTranslate() const;
    void setFillTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});

    static PropertyValue<TranslateAnchorType> getDefaultFillTranslateAnchor();
    PropertyValue<TranslateAnchorType> getFillTranslateAnchor() const;
    void setFillTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});

    static PropertyValue<std::string> getDefaultFillPattern();
    PropertyValue<std::string> getFillPattern() const;
    void setFillPattern(PropertyValue<std::string>, const optional<std::string>& klass = {});

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
