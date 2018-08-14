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

struct HeatmapRadius : DataDrivenPaintProperty<float, attributes::a_radius, uniforms::u_radius> {
    static float defaultValue() { return 30; }
};

struct HeatmapWeight : DataDrivenPaintProperty<float, attributes::a_weight, uniforms::u_weight> {
    static float defaultValue() { return 1; }
};

struct HeatmapIntensity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct HeatmapColor : ColorRampProperty {
};

struct HeatmapOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

class HeatmapPaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_radius::Type>> heatmapRadius;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_weight::Type>> heatmapWeight;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_radius>,
            ZoomInterpolatedAttribute<attributes::a_weight>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_radius>,
            InterpolationUniform<attributes::a_weight>,
            uniforms::u_radius,
            uniforms::u_weight
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        PossiblyEvaluatedPropertyValue<float> heatmapRadius;
        PossiblyEvaluatedPropertyValue<float> heatmapWeight;
        float heatmapIntensity;
        Color heatmapColor;
        float heatmapOpacity;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> heatmapRadius;
        style::Transitioning<PropertyValue<float>> heatmapWeight;
        style::Transitioning<PropertyValue<float>> heatmapIntensity;
        style::Transitioning<ColorRampPropertyValue> heatmapColor;
        style::Transitioning<PropertyValue<float>> heatmapOpacity;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> heatmapRadius;
        style::Transitionable<PropertyValue<float>> heatmapWeight;
        style::Transitionable<PropertyValue<float>> heatmapIntensity;
        style::Transitionable<ColorRampPropertyValue> heatmapColor;
        style::Transitionable<PropertyValue<float>> heatmapOpacity;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
