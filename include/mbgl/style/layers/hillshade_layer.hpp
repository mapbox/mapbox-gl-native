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
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Paint properties

    static PropertyValue<float> getDefaultHillshadeIlluminationDirection();
    PropertyValue<float> getHillshadeIlluminationDirection() const;
    void setHillshadeIlluminationDirection(PropertyValue<float>);
    void setHillshadeIlluminationDirectionTransition(const TransitionOptions&);
    TransitionOptions getHillshadeIlluminationDirectionTransition() const;

    static PropertyValue<HillshadeIlluminationAnchorType> getDefaultHillshadeIlluminationAnchor();
    PropertyValue<HillshadeIlluminationAnchorType> getHillshadeIlluminationAnchor() const;
    void setHillshadeIlluminationAnchor(PropertyValue<HillshadeIlluminationAnchorType>);
    void setHillshadeIlluminationAnchorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeIlluminationAnchorTransition() const;

    static PropertyValue<float> getDefaultHillshadeExaggeration();
    PropertyValue<float> getHillshadeExaggeration() const;
    void setHillshadeExaggeration(PropertyValue<float>);
    void setHillshadeExaggerationTransition(const TransitionOptions&);
    TransitionOptions getHillshadeExaggerationTransition() const;

    static PropertyValue<Color> getDefaultHillshadeShadowColor();
    PropertyValue<Color> getHillshadeShadowColor() const;
    void setHillshadeShadowColor(PropertyValue<Color>);
    void setHillshadeShadowColorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeShadowColorTransition() const;

    static PropertyValue<Color> getDefaultHillshadeHighlightColor();
    PropertyValue<Color> getHillshadeHighlightColor() const;
    void setHillshadeHighlightColor(PropertyValue<Color>);
    void setHillshadeHighlightColorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeHighlightColorTransition() const;

    static PropertyValue<Color> getDefaultHillshadeAccentColor();
    PropertyValue<Color> getHillshadeAccentColor() const;
    void setHillshadeAccentColor(PropertyValue<Color>);
    void setHillshadeAccentColorTransition(const TransitionOptions&);
    TransitionOptions getHillshadeAccentColorTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    HillshadeLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class HillshadeLayerFactory : public LayerFactory {
public:
    HillshadeLayerFactory();
    // LayerFactory overrides.
    ~HillshadeLayerFactory() override;
    bool supportsType(const std::string& type) const noexcept final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static HillshadeLayerFactory* get() noexcept;

private:
    static HillshadeLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
