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

struct FillAntialias : PaintProperty<bool> {
    static bool defaultValue() { return true; }
};

struct FillOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
    static float defaultValue() { return 1; }
};

struct FillColor : DataDrivenPaintProperty<Color, attributes::a_color, uniforms::u_color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillOutlineColor : DataDrivenPaintProperty<Color, attributes::a_outline_color, uniforms::u_outline_color> {
    static Color defaultValue() { return {}; }
};

struct FillTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct FillTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct FillPattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

class FillPaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_opacity::Type>> fillOpacity;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_color::Type>> fillColor;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_outline_color::Type>> fillOutlineColor;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_opacity>,
            ZoomInterpolatedAttribute<attributes::a_color>,
            ZoomInterpolatedAttribute<attributes::a_outline_color>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_opacity>,
            InterpolationUniform<attributes::a_color>,
            InterpolationUniform<attributes::a_outline_color>,
            uniforms::u_opacity,
            uniforms::u_color,
            uniforms::u_outline_color
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        bool fillAntialias;
        PossiblyEvaluatedPropertyValue<float> fillOpacity;
        PossiblyEvaluatedPropertyValue<Color> fillColor;
        PossiblyEvaluatedPropertyValue<Color> fillOutlineColor;
        std::array<float, 2> fillTranslate;
        TranslateAnchorType fillTranslateAnchor;
        Faded<std::string> fillPattern;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<bool>> fillAntialias;
        style::Transitioning<PropertyValue<float>> fillOpacity;
        style::Transitioning<PropertyValue<Color>> fillColor;
        style::Transitioning<PropertyValue<Color>> fillOutlineColor;
        style::Transitioning<PropertyValue<std::array<float, 2>>> fillTranslate;
        style::Transitioning<PropertyValue<TranslateAnchorType>> fillTranslateAnchor;
        style::Transitioning<PropertyValue<std::string>> fillPattern;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<bool>> fillAntialias;
        style::Transitionable<PropertyValue<float>> fillOpacity;
        style::Transitionable<PropertyValue<Color>> fillColor;
        style::Transitionable<PropertyValue<Color>> fillOutlineColor;
        style::Transitionable<PropertyValue<std::array<float, 2>>> fillTranslate;
        style::Transitionable<PropertyValue<TranslateAnchorType>> fillTranslateAnchor;
        style::Transitionable<PropertyValue<std::string>> fillPattern;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
