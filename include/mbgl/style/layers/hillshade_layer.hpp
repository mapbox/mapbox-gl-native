// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class HillshadeLayer : public Layer {
public:
    HillshadeLayer(const std::string& layerID, const std::string& sourceID);
    ~HillshadeLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static PropertyValue<Color> getDefaultHillshadeAccentColor();
    const PropertyValue<Color>& getHillshadeAccentColor() const;
    void setHillshadeAccentColor(const PropertyValue<Color>&);
    void setHillshadeAccentColorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeAccentColorTransition() const;

    static PropertyValue<float> getDefaultHillshadeExaggeration();
    const PropertyValue<float>& getHillshadeExaggeration() const;
    void setHillshadeExaggeration(const PropertyValue<float>&);
    void setHillshadeExaggerationTransition(const TransitionOptions&);
    TransitionOptions getHillshadeExaggerationTransition() const;

    static PropertyValue<Color> getDefaultHillshadeHighlightColor();
    const PropertyValue<Color>& getHillshadeHighlightColor() const;
    void setHillshadeHighlightColor(const PropertyValue<Color>&);
    void setHillshadeHighlightColorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeHighlightColorTransition() const;

    static PropertyValue<HillshadeIlluminationAnchorType> getDefaultHillshadeIlluminationAnchor();
    const PropertyValue<HillshadeIlluminationAnchorType>& getHillshadeIlluminationAnchor() const;
    void setHillshadeIlluminationAnchor(const PropertyValue<HillshadeIlluminationAnchorType>&);
    void setHillshadeIlluminationAnchorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeIlluminationAnchorTransition() const;

    static PropertyValue<float> getDefaultHillshadeIlluminationDirection();
    const PropertyValue<float>& getHillshadeIlluminationDirection() const;
    void setHillshadeIlluminationDirection(const PropertyValue<float>&);
    void setHillshadeIlluminationDirectionTransition(const TransitionOptions&);
    TransitionOptions getHillshadeIlluminationDirectionTransition() const;

    static PropertyValue<Color> getDefaultHillshadeShadowColor();
    const PropertyValue<Color>& getHillshadeShadowColor() const;
    void setHillshadeShadowColor(const PropertyValue<Color>&);
    void setHillshadeShadowColorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeShadowColorTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    HillshadeLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
