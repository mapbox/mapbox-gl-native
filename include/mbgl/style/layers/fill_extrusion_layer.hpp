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

class FillExtrusionLayer : public Layer {
public:
    FillExtrusionLayer(const std::string& layerID, const std::string& sourceID);
    ~FillExtrusionLayer() final;

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

    static PropertyValue<float> getDefaultFillExtrusionOpacity();
    PropertyValue<float> getFillExtrusionOpacity() const;
    void setFillExtrusionOpacity(PropertyValue<float>);
    void setFillExtrusionOpacityTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionOpacityTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultFillExtrusionColor();
    DataDrivenPropertyValue<Color> getFillExtrusionColor() const;
    void setFillExtrusionColor(DataDrivenPropertyValue<Color>);
    void setFillExtrusionColorTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionColorTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultFillExtrusionTranslate();
    PropertyValue<std::array<float, 2>> getFillExtrusionTranslate() const;
    void setFillExtrusionTranslate(PropertyValue<std::array<float, 2>>);
    void setFillExtrusionTranslateTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultFillExtrusionTranslateAnchor();
    PropertyValue<TranslateAnchorType> getFillExtrusionTranslateAnchor() const;
    void setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType>);
    void setFillExtrusionTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionTranslateAnchorTransition() const;

    static PropertyValue<std::string> getDefaultFillExtrusionPattern();
    PropertyValue<std::string> getFillExtrusionPattern() const;
    void setFillExtrusionPattern(PropertyValue<std::string>);
    void setFillExtrusionPatternTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionPatternTransition() const;

    static DataDrivenPropertyValue<float> getDefaultFillExtrusionHeight();
    DataDrivenPropertyValue<float> getFillExtrusionHeight() const;
    void setFillExtrusionHeight(DataDrivenPropertyValue<float>);
    void setFillExtrusionHeightTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionHeightTransition() const;

    static DataDrivenPropertyValue<float> getDefaultFillExtrusionBase();
    DataDrivenPropertyValue<float> getFillExtrusionBase() const;
    void setFillExtrusionBase(DataDrivenPropertyValue<float>);
    void setFillExtrusionBaseTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionBaseTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    FillExtrusionLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;
};

template <>
inline bool Layer::is<FillExtrusionLayer>() const {
    return getType() == LayerType::FillExtrusion;
}

} // namespace style
} // namespace mbgl
