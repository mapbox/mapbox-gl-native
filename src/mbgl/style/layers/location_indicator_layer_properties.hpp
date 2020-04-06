// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct BearingImage : LayoutProperty<expression::Image> {
    static constexpr const char *name() { return "bearing-image"; }
    static expression::Image defaultValue() { return {}; }
};

struct ImageTiltDisplacement : LayoutProperty<float> {
    static constexpr const char *name() { return "image-tilt-displacement"; }
    static float defaultValue() { return 0; }
};

struct PerspectiveCompensation : LayoutProperty<float> {
    static constexpr const char *name() { return "perspective-compensation"; }
    static float defaultValue() { return 0.85; }
};

struct ShadowImage : LayoutProperty<expression::Image> {
    static constexpr const char *name() { return "shadow-image"; }
    static expression::Image defaultValue() { return {}; }
};

struct TopImage : LayoutProperty<expression::Image> {
    static constexpr const char *name() { return "top-image"; }
    static expression::Image defaultValue() { return {}; }
};

struct AccuracyRadius : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct AccuracyRadiusBorderColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::white(); }
};

struct AccuracyRadiusColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::white(); }
};

struct Bearing : PaintProperty<Rotation> {
    static Rotation defaultValue() { return 0; }
};

struct BearingImageSize : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct Location : PaintProperty<std::array<double, 3>> {
    static std::array<double, 3> defaultValue() { return {{0, 0, 0}}; }
};

struct ShadowImageSize : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct TopImageSize : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

class LocationIndicatorLayoutProperties : public Properties<
    BearingImage,
    ImageTiltDisplacement,
    PerspectiveCompensation,
    ShadowImage,
    TopImage
> {};

class LocationIndicatorPaintProperties : public Properties<
    AccuracyRadius,
    AccuracyRadiusBorderColor,
    AccuracyRadiusColor,
    Bearing,
    BearingImageSize,
    Location,
    ShadowImageSize,
    TopImageSize
> {};

class LocationIndicatorLayerProperties final : public LayerProperties {
public:
    explicit LocationIndicatorLayerProperties(Immutable<LocationIndicatorLayer::Impl>);
    LocationIndicatorLayerProperties(
        Immutable<LocationIndicatorLayer::Impl>,
        LocationIndicatorPaintProperties::PossiblyEvaluated);
    ~LocationIndicatorLayerProperties() override;

    unsigned long constantsMask() const override;

    const LocationIndicatorLayer::Impl& layerImpl() const;
    // Data members.
    LocationIndicatorPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl

// clang-format on
