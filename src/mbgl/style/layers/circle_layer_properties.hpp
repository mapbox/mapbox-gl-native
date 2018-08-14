// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct CircleRadius : DataDrivenPaintProperty<float, attributes::a_radius, uniforms::u_radius> {
    static float defaultValue() { return 5; }
};

struct CircleColor : DataDrivenPaintProperty<Color, attributes::a_color, uniforms::u_color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleBlur : DataDrivenPaintProperty<float, attributes::a_blur, uniforms::u_blur> {
    static float defaultValue() { return 0; }
};

struct CircleOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
    static float defaultValue() { return 1; }
};

struct CircleTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct CircleTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct CirclePitchScale : PaintProperty<CirclePitchScaleType> {
    static CirclePitchScaleType defaultValue() { return CirclePitchScaleType::Map; }
};

struct CirclePitchAlignment : PaintProperty<AlignmentType> {
    static AlignmentType defaultValue() { return AlignmentType::Viewport; }
};

struct CircleStrokeWidth : DataDrivenPaintProperty<float, attributes::a_stroke_width, uniforms::u_stroke_width> {
    static float defaultValue() { return 0; }
};

struct CircleStrokeColor : DataDrivenPaintProperty<Color, attributes::a_stroke_color, uniforms::u_stroke_color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleStrokeOpacity : DataDrivenPaintProperty<float, attributes::a_stroke_opacity, uniforms::u_stroke_opacity> {
    static float defaultValue() { return 1; }
};

class CirclePaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_radius::Type>> circleRadius;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_color::Type>> circleColor;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_blur::Type>> circleBlur;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_opacity::Type>> circleOpacity;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_stroke_width::Type>> circleStrokeWidth;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_stroke_color::Type>> circleStrokeColor;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_stroke_opacity::Type>> circleStrokeOpacity;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_radius>,
            ZoomInterpolatedAttribute<attributes::a_color>,
            ZoomInterpolatedAttribute<attributes::a_blur>,
            ZoomInterpolatedAttribute<attributes::a_opacity>,
            ZoomInterpolatedAttribute<attributes::a_stroke_width>,
            ZoomInterpolatedAttribute<attributes::a_stroke_color>,
            ZoomInterpolatedAttribute<attributes::a_stroke_opacity>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_radius>,
            InterpolationUniform<attributes::a_color>,
            InterpolationUniform<attributes::a_blur>,
            InterpolationUniform<attributes::a_opacity>,
            InterpolationUniform<attributes::a_stroke_width>,
            InterpolationUniform<attributes::a_stroke_color>,
            InterpolationUniform<attributes::a_stroke_opacity>,
            uniforms::u_radius,
            uniforms::u_color,
            uniforms::u_blur,
            uniforms::u_opacity,
            uniforms::u_stroke_width,
            uniforms::u_stroke_color,
            uniforms::u_stroke_opacity
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        PossiblyEvaluatedPropertyValue<float> circleRadius;
        PossiblyEvaluatedPropertyValue<Color> circleColor;
        PossiblyEvaluatedPropertyValue<float> circleBlur;
        PossiblyEvaluatedPropertyValue<float> circleOpacity;
        std::array<float, 2> circleTranslate;
        TranslateAnchorType circleTranslateAnchor;
        CirclePitchScaleType circlePitchScale;
        AlignmentType circlePitchAlignment;
        PossiblyEvaluatedPropertyValue<float> circleStrokeWidth;
        PossiblyEvaluatedPropertyValue<Color> circleStrokeColor;
        PossiblyEvaluatedPropertyValue<float> circleStrokeOpacity;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> circleRadius;
        style::Transitioning<PropertyValue<Color>> circleColor;
        style::Transitioning<PropertyValue<float>> circleBlur;
        style::Transitioning<PropertyValue<float>> circleOpacity;
        style::Transitioning<PropertyValue<std::array<float, 2>>> circleTranslate;
        style::Transitioning<PropertyValue<TranslateAnchorType>> circleTranslateAnchor;
        style::Transitioning<PropertyValue<CirclePitchScaleType>> circlePitchScale;
        style::Transitioning<PropertyValue<AlignmentType>> circlePitchAlignment;
        style::Transitioning<PropertyValue<float>> circleStrokeWidth;
        style::Transitioning<PropertyValue<Color>> circleStrokeColor;
        style::Transitioning<PropertyValue<float>> circleStrokeOpacity;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> circleRadius;
        style::Transitionable<PropertyValue<Color>> circleColor;
        style::Transitionable<PropertyValue<float>> circleBlur;
        style::Transitionable<PropertyValue<float>> circleOpacity;
        style::Transitionable<PropertyValue<std::array<float, 2>>> circleTranslate;
        style::Transitionable<PropertyValue<TranslateAnchorType>> circleTranslateAnchor;
        style::Transitionable<PropertyValue<CirclePitchScaleType>> circlePitchScale;
        style::Transitionable<PropertyValue<AlignmentType>> circlePitchAlignment;
        style::Transitionable<PropertyValue<float>> circleStrokeWidth;
        style::Transitionable<PropertyValue<Color>> circleStrokeColor;
        style::Transitionable<PropertyValue<float>> circleStrokeOpacity;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
