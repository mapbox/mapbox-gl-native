// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

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

    // Visibility
    void setVisibility(VisibilityType) final;

    // Zoom range
    void setMinZoom(float) final;
    void setMaxZoom(float) final;

    // Paint properties

    static PropertyValue<bool> getDefaultFillAntialias();
    PropertyValue<bool> getFillAntialias() const;
    void setFillAntialias(PropertyValue<bool>);
    void setFillAntialiasTransition(const TransitionOptions&);
    TransitionOptions getFillAntialiasTransition() const;

    static DataDrivenPropertyValue<float> getDefaultFillOpacity();
    DataDrivenPropertyValue<float> getFillOpacity() const;
    void setFillOpacity(DataDrivenPropertyValue<float>);
    void setFillOpacityTransition(const TransitionOptions&);
    TransitionOptions getFillOpacityTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultFillColor();
    DataDrivenPropertyValue<Color> getFillColor() const;
    void setFillColor(DataDrivenPropertyValue<Color>);
    void setFillColorTransition(const TransitionOptions&);
    TransitionOptions getFillColorTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultFillOutlineColor();
    DataDrivenPropertyValue<Color> getFillOutlineColor() const;
    void setFillOutlineColor(DataDrivenPropertyValue<Color>);
    void setFillOutlineColorTransition(const TransitionOptions&);
    TransitionOptions getFillOutlineColorTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultFillTranslate();
    PropertyValue<std::array<float, 2>> getFillTranslate() const;
    void setFillTranslate(PropertyValue<std::array<float, 2>>);
    void setFillTranslateTransition(const TransitionOptions&);
    TransitionOptions getFillTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultFillTranslateAnchor();
    PropertyValue<TranslateAnchorType> getFillTranslateAnchor() const;
    void setFillTranslateAnchor(PropertyValue<TranslateAnchorType>);
    void setFillTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getFillTranslateAnchorTransition() const;

    static PropertyValue<std::string> getDefaultFillPattern();
    PropertyValue<std::string> getFillPattern() const;
    void setFillPattern(PropertyValue<std::string>);
    void setFillPatternTransition(const TransitionOptions&);
    TransitionOptions getFillPatternTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    FillLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;
};

template <>
inline bool Layer::is<FillLayer>() const {
    return getType() == LayerType::Fill;
}

} // namespace style
} // namespace mbgl
