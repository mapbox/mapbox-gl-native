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

struct FillExtrusionOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct FillExtrusionColor : DataDrivenPaintProperty<Color, attributes::a_color, uniforms::u_color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillExtrusionTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct FillExtrusionTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct FillExtrusionPattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

struct FillExtrusionHeight : DataDrivenPaintProperty<float, attributes::a_height, uniforms::u_height> {
    static float defaultValue() { return 0; }
};

struct FillExtrusionBase : DataDrivenPaintProperty<float, attributes::a_base, uniforms::u_base> {
    static float defaultValue() { return 0; }
};

class FillExtrusionPaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_color::Type>> fillExtrusionColor;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_height::Type>> fillExtrusionHeight;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_base::Type>> fillExtrusionBase;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_color>,
            ZoomInterpolatedAttribute<attributes::a_height>,
            ZoomInterpolatedAttribute<attributes::a_base>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_color>,
            InterpolationUniform<attributes::a_height>,
            InterpolationUniform<attributes::a_base>,
            uniforms::u_color,
            uniforms::u_height,
            uniforms::u_base
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        float fillExtrusionOpacity;
        PossiblyEvaluatedPropertyValue<Color> fillExtrusionColor;
        std::array<float, 2> fillExtrusionTranslate;
        TranslateAnchorType fillExtrusionTranslateAnchor;
        Faded<std::string> fillExtrusionPattern;
        PossiblyEvaluatedPropertyValue<float> fillExtrusionHeight;
        PossiblyEvaluatedPropertyValue<float> fillExtrusionBase;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> fillExtrusionOpacity;
        style::Transitioning<PropertyValue<Color>> fillExtrusionColor;
        style::Transitioning<PropertyValue<std::array<float, 2>>> fillExtrusionTranslate;
        style::Transitioning<PropertyValue<TranslateAnchorType>> fillExtrusionTranslateAnchor;
        style::Transitioning<PropertyValue<std::string>> fillExtrusionPattern;
        style::Transitioning<PropertyValue<float>> fillExtrusionHeight;
        style::Transitioning<PropertyValue<float>> fillExtrusionBase;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> fillExtrusionOpacity;
        style::Transitionable<PropertyValue<Color>> fillExtrusionColor;
        style::Transitionable<PropertyValue<std::array<float, 2>>> fillExtrusionTranslate;
        style::Transitionable<PropertyValue<TranslateAnchorType>> fillExtrusionTranslateAnchor;
        style::Transitionable<PropertyValue<std::string>> fillExtrusionPattern;
        style::Transitionable<PropertyValue<float>> fillExtrusionHeight;
        style::Transitionable<PropertyValue<float>> fillExtrusionBase;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
