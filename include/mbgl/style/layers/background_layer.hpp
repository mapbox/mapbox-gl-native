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

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(const std::string& layerID);
    ~BackgroundLayer() final;

    // Dynamic properties
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Paint properties

    static PropertyValue<Color> getDefaultBackgroundColor();
    PropertyValue<Color> getBackgroundColor() const;
    void setBackgroundColor(PropertyValue<Color>);
    void setBackgroundColorTransition(const TransitionOptions&);
    TransitionOptions getBackgroundColorTransition() const;

    static PropertyValue<std::string> getDefaultBackgroundPattern();
    PropertyValue<std::string> getBackgroundPattern() const;
    void setBackgroundPattern(PropertyValue<std::string>);
    void setBackgroundPatternTransition(const TransitionOptions&);
    TransitionOptions getBackgroundPatternTransition() const;

    static PropertyValue<float> getDefaultBackgroundOpacity();
    PropertyValue<float> getBackgroundOpacity() const;
    void setBackgroundOpacity(PropertyValue<float>);
    void setBackgroundOpacityTransition(const TransitionOptions&);
    TransitionOptions getBackgroundOpacityTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    BackgroundLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class BackgroundLayerFactory : public LayerFactory {
public:
    BackgroundLayerFactory();
    // LayerFactory overrides.
    ~BackgroundLayerFactory() override;
    bool supportsType(const std::string& type) const final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static BackgroundLayerFactory* get();

private:
    static BackgroundLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
