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

class FillExtrusionLayer : public Layer {
public:
    FillExtrusionLayer(const std::string& layerID, const std::string& sourceID);
    ~FillExtrusionLayer() final;

    // Dynamic properties
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Paint properties

    static PropertyValue<float> getDefaultFillExtrusionOpacity();
    PropertyValue<float> getFillExtrusionOpacity() const;
    void setFillExtrusionOpacity(PropertyValue<float>);
    void setFillExtrusionOpacityTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionOpacityTransition() const;

    static PropertyValue<Color> getDefaultFillExtrusionColor();
    PropertyValue<Color> getFillExtrusionColor() const;
    void setFillExtrusionColor(PropertyValue<Color>);
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

    static PropertyValue<float> getDefaultFillExtrusionHeight();
    PropertyValue<float> getFillExtrusionHeight() const;
    void setFillExtrusionHeight(PropertyValue<float>);
    void setFillExtrusionHeightTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionHeightTransition() const;

    static PropertyValue<float> getDefaultFillExtrusionBase();
    PropertyValue<float> getFillExtrusionBase() const;
    void setFillExtrusionBase(PropertyValue<float>);
    void setFillExtrusionBaseTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionBaseTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    FillExtrusionLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class FillExtrusionLayerFactory : public LayerFactory {
public:
    FillExtrusionLayerFactory();
    // LayerFactory overrides.
    ~FillExtrusionLayerFactory() override;
    bool supportsType(const std::string& type) const final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static FillExtrusionLayerFactory* get();

private:
    static FillExtrusionLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
