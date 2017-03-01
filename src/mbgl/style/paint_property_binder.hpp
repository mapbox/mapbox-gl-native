#pragma once

#include <mbgl/programs/attributes.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/type_list.hpp>

namespace mbgl {
namespace style {

template <class T, class A>
class ConstantPaintPropertyBinder {
public:
    using Attribute = A;
    using AttributeValue = typename Attribute::Value;
    using AttributeBinding = typename Attribute::Binding;

    ConstantPaintPropertyBinder(T constant_)
        : constant(std::move(constant_)) {
    }

    void populateVertexVector(const GeometryTileFeature&, std::size_t) {}
    void upload(gl::Context&) {}

    AttributeBinding minAttributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const {
        return typename Attribute::ConstantBinding {
            Attribute::value(currentValue.constantOr(constant))
        };
    }

    AttributeBinding maxAttributeBinding(const PossiblyEvaluatedPropertyValue<T>&) const {
        return AttributeBinding();
    }

    float interpolationFactor(float) const {
        return 0.0f;
    }

private:
    T constant;
};

template <class T, class A>
class SourceFunctionPaintPropertyBinder {
public:
    using Attribute = A;
    using AttributeValue = typename Attribute::Value;
    using AttributeBinding = typename Attribute::Binding;

    using Attributes = gl::Attributes<Attribute>;
    using Vertex = typename Attributes::Vertex;

    SourceFunctionPaintPropertyBinder(SourceFunction<T> function_, T defaultValue_)
        : function(std::move(function_)),
          defaultValue(std::move(defaultValue_)) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) {
        AttributeValue value = Attribute::value(function.evaluate(feature, defaultValue));
        for (std::size_t i = vertexVector.vertexSize(); i < length; ++i) {
            vertexVector.emplace_back(Vertex { value });
        }
    }

    void upload(gl::Context& context) {
        vertexBuffer = context.createVertexBuffer(std::move(vertexVector));
    }

    AttributeBinding minAttributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const {
        if (currentValue.isConstant()) {
            return typename Attribute::ConstantBinding {
                Attribute::value(*currentValue.constant())
            };
        } else {
            return Attributes::allVariableBindings(*vertexBuffer)
                .template get<Attribute>();
        }
    }

    AttributeBinding maxAttributeBinding(const PossiblyEvaluatedPropertyValue<T>&) const {
        return AttributeBinding();
    }

    float interpolationFactor(float) const {
        return 0.0f;
    }

private:
    SourceFunction<T> function;
    T defaultValue;
    gl::VertexVector<Vertex> vertexVector;
    optional<gl::VertexBuffer<Vertex>> vertexBuffer;
};

template <class T, class A>
class CompositeFunctionPaintPropertyBinder {
public:
    using Attribute = A;
    using AttributeValue = typename Attribute::Value;
    using AttributeBinding = typename Attribute::Binding;

    using MinAttribute = attributes::Min<Attribute>;
    using MaxAttribute = attributes::Max<Attribute>;

    using Attributes = gl::Attributes<MinAttribute, MaxAttribute>;
    using Vertex = typename Attributes::Vertex;

    CompositeFunctionPaintPropertyBinder(CompositeFunction<T> function_, float zoom, T defaultValue_)
        : function(std::move(function_)),
          defaultValue(std::move(defaultValue_)),
          coveringRanges(function.coveringRanges(zoom)) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) {
        Range<T> range = function.evaluate(std::get<1>(coveringRanges), feature, defaultValue);
        AttributeValue min = Attribute::value(range.min);
        AttributeValue max = Attribute::value(range.max);
        for (std::size_t i = vertexVector.vertexSize(); i < length; ++i) {
            vertexVector.emplace_back(Vertex { min, max });
        }
    }

    void upload(gl::Context& context) {
        vertexBuffer = context.createVertexBuffer(std::move(vertexVector));
    }

    AttributeBinding minAttributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const {
        if (currentValue.isConstant()) {
            return typename Attribute::ConstantBinding {
                Attribute::value(*currentValue.constant())
            };
        } else {
            return Attributes::allVariableBindings(*vertexBuffer)
                .template get<MinAttribute>();
        }
    }

    AttributeBinding maxAttributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const {
        if (currentValue.isConstant()) {
            return AttributeBinding();
        } else {
            return Attributes::allVariableBindings(*vertexBuffer)
                .template get<MaxAttribute>();
        }
    }

    float interpolationFactor(float currentZoom) const {
        return util::interpolationFactor(1.0f, std::get<0>(coveringRanges), currentZoom);
    }

private:
    using InnerStops = typename CompositeFunction<T>::InnerStops;
    CompositeFunction<T> function;
    T defaultValue;
    std::tuple<Range<float>, Range<InnerStops>> coveringRanges;
    gl::VertexVector<Vertex> vertexVector;
    optional<gl::VertexBuffer<Vertex>> vertexBuffer;
};

template <class PaintProperty>
class PaintPropertyBinder {
public:
    using Type = typename PaintProperty::Type;
    using Attribute = typename PaintProperty::Attribute;
    using PropertyValue = typename PaintProperty::EvaluatedType;

    using Binder = variant<
        ConstantPaintPropertyBinder<Type, Attribute>,
        SourceFunctionPaintPropertyBinder<Type, Attribute>,
        CompositeFunctionPaintPropertyBinder<Type, Attribute>>;

    PaintPropertyBinder(const PropertyValue& value, float zoom)
        : binder(value.match(
            [&] (const Type& constant) -> Binder {
                return ConstantPaintPropertyBinder<Type, Attribute>(constant);
            },
            [&] (const SourceFunction<Type>& function) {
                return SourceFunctionPaintPropertyBinder<Type, Attribute>(function, PaintProperty::defaultValue());
            },
            [&] (const CompositeFunction<Type>& function) {
                return CompositeFunctionPaintPropertyBinder<Type, Attribute>(function, zoom, PaintProperty::defaultValue());
            }
        )) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) {
        binder.match([&] (auto& b) {
            b.populateVertexVector(feature, length);
        });
    }

    void upload(gl::Context& context) {
        binder.match([&] (auto& b) {
            b.upload(context);
        });
    }

    using MinAttribute = attributes::Min<Attribute>;
    using MaxAttribute = attributes::Max<Attribute>;
    using AttributeBinding = typename Attribute::Binding;

    AttributeBinding minAttributeBinding(const PropertyValue& currentValue) const {
        return binder.match([&] (const auto& b) {
            return b.minAttributeBinding(currentValue);
        });
    }

    AttributeBinding maxAttributeBinding(const PropertyValue& currentValue) const {
        return binder.match([&] (const auto& b) {
            return b.maxAttributeBinding(currentValue);
        });
    }

    using InterpolationUniform = attributes::InterpolationUniform<Attribute>;
    using InterpolationUniformValue = typename InterpolationUniform::Value;

    InterpolationUniformValue interpolationUniformValue(float currentZoom) const {
        return InterpolationUniformValue {
            binder.match([&] (const auto& b) {
                return b.interpolationFactor(currentZoom);
            })
        };
    }

private:
    Binder binder;
};

template <class Ps>
class PaintPropertyBinders;

template <class... Ps>
class PaintPropertyBinders<TypeList<Ps...>> {
public:
    using Binders = IndexedTuple<TypeList<Ps...>, TypeList<PaintPropertyBinder<Ps>...>>;

    template <class EvaluatedProperties>
    PaintPropertyBinders(const EvaluatedProperties& properties, float z)
        : binders(PaintPropertyBinder<Ps>(properties.template get<Ps>(), z)...) {
        (void)z; // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56958
    }

    void populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
        util::ignore({
            (binders.template get<Ps>().populateVertexVector(feature, length), 0)...
        });
    }

    void upload(gl::Context& context) {
        util::ignore({
            (binders.template get<Ps>().upload(context), 0)...
        });
    }

    using MinAttributes = gl::Attributes<typename PaintPropertyBinder<Ps>::MinAttribute...>;
    using MaxAttributes = gl::Attributes<typename PaintPropertyBinder<Ps>::MaxAttribute...>;

    using Attributes = gl::ConcatenateAttributes<MinAttributes, MaxAttributes>;
    using AttributeBindings = typename Attributes::Bindings;

    template <class EvaluatedProperties>
    AttributeBindings attributeBindings(const EvaluatedProperties& currentProperties) const {
        const typename MinAttributes::Bindings min {
            binders.template get<Ps>().minAttributeBinding(currentProperties.template get<Ps>())...
        };
        const typename MaxAttributes::Bindings max {
            binders.template get<Ps>().maxAttributeBinding(currentProperties.template get<Ps>())...
        };
        return min.concat(max);
    }

    using Uniforms = gl::Uniforms<typename PaintPropertyBinder<Ps>::InterpolationUniform...>;
    using UniformValues = typename Uniforms::Values;

    UniformValues uniformValues(float currentZoom) const {
        (void)currentZoom; // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56958
        return UniformValues {
            binders.template get<Ps>().interpolationUniformValue(currentZoom)...
        };
    }

private:
    Binders binders;
};

} // namespace style
} // namespace mbgl
