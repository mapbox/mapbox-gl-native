// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/location_component_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct AccuracyRadius : LayoutProperty<float> {
    static constexpr const char *name() { return "accuracy-radius"; }
    static float defaultValue() { return 0; }
};

struct Bearing : LayoutProperty<float> {
    static constexpr const char *name() { return "bearing"; }
    static float defaultValue() { return 0; }
};

struct BearingImage : LayoutProperty<expression::Image> {
    static constexpr const char *name() { return "bearing-image"; }
    static expression::Image defaultValue() { return {}; }
};

struct BearingImageSize : LayoutProperty<float> {
    static constexpr const char *name() { return "bearing-image-size"; }
    static float defaultValue() { return 1; }
};

struct ImageTiltDisplacement : LayoutProperty<float> {
    static constexpr const char *name() { return "image-tilt-displacement"; }
    static float defaultValue() { return 0; }
};

struct Location : LayoutProperty<std::array<double, 3>> {
    static constexpr const char *name() { return "location"; }
    static std::array<double, 3> defaultValue() { return {{0, 0, 0}}; }
};

struct PerspectiveCompensation : LayoutProperty<float> {
    static constexpr const char *name() { return "perspective-compensation"; }
    static float defaultValue() { return 0.85; }
};

struct ShadowImage : LayoutProperty<expression::Image> {
    static constexpr const char *name() { return "shadow-image"; }
    static expression::Image defaultValue() { return {}; }
};

struct ShadowImageSize : LayoutProperty<float> {
    static constexpr const char *name() { return "shadow-image-size"; }
    static float defaultValue() { return 1; }
};

struct TopImage : LayoutProperty<expression::Image> {
    static constexpr const char *name() { return "top-image"; }
    static expression::Image defaultValue() { return {}; }
};

struct TopImageSize : LayoutProperty<float> {
    static constexpr const char *name() { return "top-image-size"; }
    static float defaultValue() { return 1; }
};

struct AccuracyRadiusBorderColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::white(); }
};

struct AccuracyRadiusColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::white(); }
};

class LocationComponentLayoutProperties : public Properties<
    AccuracyRadius,
    Bearing,
    BearingImage,
    BearingImageSize,
    ImageTiltDisplacement,
    Location,
    PerspectiveCompensation,
    ShadowImage,
    ShadowImageSize,
    TopImage,
    TopImageSize
> {};

class LocationComponentPaintProperties : public Properties<
    AccuracyRadiusBorderColor,
    AccuracyRadiusColor
> {};

class LocationComponentLayerProperties final : public LayerProperties {
public:
    explicit LocationComponentLayerProperties(Immutable<LocationComponentLayer::Impl>);
    LocationComponentLayerProperties(
        Immutable<LocationComponentLayer::Impl>,
        LocationComponentPaintProperties::PossiblyEvaluated);
    ~LocationComponentLayerProperties() override;

    unsigned long constantsMask() const override;

    const LocationComponentLayer::Impl& layerImpl() const;
    // Data members.
    LocationComponentPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl

// clang-format on
