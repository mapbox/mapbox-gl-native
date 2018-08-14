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

struct LineCap : LayoutProperty<LineCapType> {
    static constexpr const char * key = "line-cap";
    static LineCapType defaultValue() { return LineCapType::Butt; }
};

struct LineJoin : DataDrivenLayoutProperty<LineJoinType> {
    static constexpr const char * key = "line-join";
    static LineJoinType defaultValue() { return LineJoinType::Miter; }
};

struct LineMiterLimit : LayoutProperty<float> {
    static constexpr const char * key = "line-miter-limit";
    static float defaultValue() { return 2; }
};

struct LineRoundLimit : LayoutProperty<float> {
    static constexpr const char * key = "line-round-limit";
    static float defaultValue() { return 1; }
};

struct LineOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
    static float defaultValue() { return 1; }
};

struct LineColor : DataDrivenPaintProperty<Color, attributes::a_color, uniforms::u_color> {
    static Color defaultValue() { return Color::black(); }
};

struct LineTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct LineTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct LineWidth : DataDrivenPaintProperty<float, attributes::a_width, uniforms::u_width> {
    static float defaultValue() { return 1; }
};

struct LineGapWidth : DataDrivenPaintProperty<float, attributes::a_gapwidth, uniforms::u_gapwidth> {
    static float defaultValue() { return 0; }
};

struct LineOffset : DataDrivenPaintProperty<float, attributes::a_offset<1>, uniforms::u_offset> {
    static float defaultValue() { return 0; }
};

struct LineBlur : DataDrivenPaintProperty<float, attributes::a_blur, uniforms::u_blur> {
    static float defaultValue() { return 0; }
};

struct LineDasharray : CrossFadedPaintProperty<std::vector<float>> {
    static std::vector<float> defaultValue() { return {  }; }
};

struct LinePattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

class LineLayoutProperties {
public:
    class Evaluated {
    public:
        LineCapType lineCap;
        LineJoinType lineJoin;
        float lineMiterLimit;
        float lineRoundLimit;
    };

    class PossiblyEvaluated {
    public:
        LineCapType lineCap;
        PossiblyEvaluatedPropertyValue<LineJoinType> lineJoin;
        float lineMiterLimit;
        float lineRoundLimit;

        Evaluated evaluate(float z, const GeometryTileFeature& feature) const;
    };

    class Unevaluated {
    public:
        PropertyValue<LineCapType> lineCap;
        PropertyValue<LineJoinType> lineJoin;
        PropertyValue<float> lineMiterLimit;
        PropertyValue<float> lineRoundLimit;

        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;

        friend bool operator==(const Unevaluated&, const Unevaluated&);
        friend inline bool operator!=(const Unevaluated& lhs, const Unevaluated& rhs) { return !(lhs == rhs); }
    };
};

class LinePaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_opacity::Type>> lineOpacity;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_color::Type>> lineColor;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_width::Type>> lineWidth;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_gapwidth::Type>> lineGapWidth;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_offset<1>::Type>> lineOffset;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_blur::Type>> lineBlur;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_opacity>,
            ZoomInterpolatedAttribute<attributes::a_color>,
            ZoomInterpolatedAttribute<attributes::a_width>,
            ZoomInterpolatedAttribute<attributes::a_gapwidth>,
            ZoomInterpolatedAttribute<attributes::a_offset<1>>,
            ZoomInterpolatedAttribute<attributes::a_blur>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_opacity>,
            InterpolationUniform<attributes::a_color>,
            InterpolationUniform<attributes::a_width>,
            InterpolationUniform<attributes::a_gapwidth>,
            InterpolationUniform<attributes::a_offset<1>>,
            InterpolationUniform<attributes::a_blur>,
            uniforms::u_opacity,
            uniforms::u_color,
            uniforms::u_width,
            uniforms::u_gapwidth,
            uniforms::u_offset,
            uniforms::u_blur
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        PossiblyEvaluatedPropertyValue<float> lineOpacity;
        PossiblyEvaluatedPropertyValue<Color> lineColor;
        std::array<float, 2> lineTranslate;
        TranslateAnchorType lineTranslateAnchor;
        PossiblyEvaluatedPropertyValue<float> lineWidth;
        PossiblyEvaluatedPropertyValue<float> lineGapWidth;
        PossiblyEvaluatedPropertyValue<float> lineOffset;
        PossiblyEvaluatedPropertyValue<float> lineBlur;
        Faded<std::vector<float>> lineDasharray;
        Faded<std::string> linePattern;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> lineOpacity;
        style::Transitioning<PropertyValue<Color>> lineColor;
        style::Transitioning<PropertyValue<std::array<float, 2>>> lineTranslate;
        style::Transitioning<PropertyValue<TranslateAnchorType>> lineTranslateAnchor;
        style::Transitioning<PropertyValue<float>> lineWidth;
        style::Transitioning<PropertyValue<float>> lineGapWidth;
        style::Transitioning<PropertyValue<float>> lineOffset;
        style::Transitioning<PropertyValue<float>> lineBlur;
        style::Transitioning<PropertyValue<std::vector<float>>> lineDasharray;
        style::Transitioning<PropertyValue<std::string>> linePattern;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> lineOpacity;
        style::Transitionable<PropertyValue<Color>> lineColor;
        style::Transitionable<PropertyValue<std::array<float, 2>>> lineTranslate;
        style::Transitionable<PropertyValue<TranslateAnchorType>> lineTranslateAnchor;
        style::Transitionable<PropertyValue<float>> lineWidth;
        style::Transitionable<PropertyValue<float>> lineGapWidth;
        style::Transitionable<PropertyValue<float>> lineOffset;
        style::Transitionable<PropertyValue<float>> lineBlur;
        style::Transitionable<PropertyValue<std::vector<float>>> lineDasharray;
        style::Transitionable<PropertyValue<std::string>> linePattern;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
