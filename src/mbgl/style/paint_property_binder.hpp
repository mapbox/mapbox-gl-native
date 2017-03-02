#pragma once

#include <mbgl/programs/attributes.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/type_list.hpp>

namespace mbgl {
namespace style {

template <class T, class A>
class PaintPropertyBinder {
public:
    using BaseAttribute = A;
    using Attribute = attributes::ZoomInterpolatedAttribute<BaseAttribute>;
    using AttributeBinding = typename Attribute::Binding;

    virtual ~PaintPropertyBinder() = default;

    virtual void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) = 0;
    virtual void upload(gl::Context& context) = 0;
    virtual AttributeBinding attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const = 0;
    virtual float interpolationFactor(float currentZoom) const = 0;
};

template <class T, class A>
class ConstantPaintPropertyBinder : public PaintPropertyBinder<T, A> {
public:
    using BaseAttribute = A;
    using BaseAttributeValue = typename BaseAttribute::Value;
    using BaseAttributeBinding = typename BaseAttribute::Binding;

    using Attribute = attributes::ZoomInterpolatedAttribute<BaseAttribute>;
    using AttributeBinding = typename Attribute::Binding;

    ConstantPaintPropertyBinder(T constant_)
        : constant(std::move(constant_)) {
    }

    void populateVertexVector(const GeometryTileFeature&, std::size_t) override {}
    void upload(gl::Context&) override {}

    AttributeBinding attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        auto val = currentValue.constantOr(constant);
        return typename Attribute::ConstantBinding {
            Attribute::value(val, val)
        };
    }

    float interpolationFactor(float) const override {
        return 0.0f;
    }

private:
    T constant;
};

template <class T, class A>
class SourceFunctionPaintPropertyBinder : public PaintPropertyBinder<T, A> {
public:
    using BaseAttribute = A;
    using BaseAttributeValue = typename BaseAttribute::Value;
    using BaseAttributeBinding = typename BaseAttribute::Binding;
    using BaseVertex = typename gl::Attributes<BaseAttribute>::Vertex;

    using Attribute = attributes::ZoomInterpolatedAttribute<BaseAttribute>;
    using AttributeBinding = typename Attribute::Binding;

    SourceFunctionPaintPropertyBinder(SourceFunction<T> function_, T defaultValue_)
        : function(std::move(function_)),
          defaultValue(std::move(defaultValue_)) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) override {
        auto val = function.evaluate(feature, defaultValue);
        BaseAttributeValue value = BaseAttribute::value(val);
        for (std::size_t i = vertexVector.vertexSize(); i < length; ++i) {
            vertexVector.emplace_back(BaseVertex { value });
        }
    }

    void upload(gl::Context& context) override {
        vertexBuffer = context.createVertexBuffer(std::move(vertexVector));
    }

    AttributeBinding attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            auto val = *currentValue.constant();
            return typename Attribute::ConstantBinding {
                Attribute::value(val, val)
            };
        } else {
            return Attribute::variableBinding(*vertexBuffer, 0, BaseAttribute::Dimensions);
        }
    }

    float interpolationFactor(float) const override {
        return 0.0f;
    }

private:
    SourceFunction<T> function;
    T defaultValue;
    gl::VertexVector<BaseVertex> vertexVector;
    optional<gl::VertexBuffer<BaseVertex>> vertexBuffer;
};

template <class T, class A>
class CompositeFunctionPaintPropertyBinder : public PaintPropertyBinder<T, A> {
public:
    using Attribute = attributes::ZoomInterpolatedAttribute<A>;
    using AttributeValue = typename Attribute::Value;
    using AttributeBinding = typename Attribute::Binding;
    using Vertex = typename gl::Attributes<Attribute>::Vertex;

    CompositeFunctionPaintPropertyBinder(CompositeFunction<T> function_, float zoom, T defaultValue_)
        : function(std::move(function_)),
          defaultValue(std::move(defaultValue_)),
          coveringRanges(function.coveringRanges(zoom)) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) override {
        Range<T> range = function.evaluate(std::get<1>(coveringRanges), feature, defaultValue);
        AttributeValue minMax = Attribute::value(range.min, range.max);
        for (std::size_t i = vertexVector.vertexSize(); i < length; ++i) {
            vertexVector.emplace_back(Vertex { minMax });
        }
    }

    void upload(gl::Context& context) override {
        vertexBuffer = context.createVertexBuffer(std::move(vertexVector));
    }

    AttributeBinding attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            auto val = *currentValue.constant();
            return typename Attribute::ConstantBinding {
                Attribute::value(val, val)
            };
        } else {
            return Attribute::variableBinding(*vertexBuffer, 0);
        }
    }

    float interpolationFactor(float currentZoom) const override {
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
auto createPaintPropertyBinder(const typename PaintProperty::EvaluatedType& value, float zoom) {
    using Type = typename PaintProperty::Type;
    using Attribute = typename PaintProperty::Attribute;
    return value.match(
        [&] (const Type& constant) -> std::unique_ptr<PaintPropertyBinder<Type, Attribute>> {
            return std::make_unique<ConstantPaintPropertyBinder<Type, Attribute>>(constant);
        },
        [&] (const SourceFunction<Type>& function) {
            return std::make_unique<SourceFunctionPaintPropertyBinder<Type, Attribute>>(function, PaintProperty::defaultValue());
        },
        [&] (const CompositeFunction<Type>& function) {
            return std::make_unique<CompositeFunctionPaintPropertyBinder<Type, Attribute>>(function, zoom, PaintProperty::defaultValue());
        }
    );
}

template <class Ps>
class PaintPropertyBinders;

template <class... Ps>
class PaintPropertyBinders<TypeList<Ps...>> {
public:
    using Binders = IndexedTuple<
        TypeList<Ps...>,
        TypeList<std::unique_ptr<PaintPropertyBinder<typename Ps::Type, typename Ps::Attribute>>...>>;

    template <class EvaluatedProperties>
    PaintPropertyBinders(const EvaluatedProperties& properties, float z)
        : binders(createPaintPropertyBinder<Ps>(properties.template get<Ps>(), z)...) {
        (void)z; // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56958
    }

    PaintPropertyBinders(PaintPropertyBinders&&) = default;
    PaintPropertyBinders(const PaintPropertyBinders&) = delete;

    void populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
        util::ignore({
            (binders.template get<Ps>()->populateVertexVector(feature, length), 0)...
        });
    }

    void upload(gl::Context& context) {
        util::ignore({
            (binders.template get<Ps>()->upload(context), 0)...
        });
    }

    using Attributes = gl::Attributes<typename PaintPropertyBinder<typename Ps::Type, typename Ps::Attribute>::Attribute...>;
    using AttributeBindings = typename Attributes::Bindings;

    template <class EvaluatedProperties>
    AttributeBindings attributeBindings(const EvaluatedProperties& currentProperties) const {
        return AttributeBindings {
            binders.template get<Ps>()->attributeBinding(currentProperties.template get<Ps>())...
        };
    }

    using Uniforms = gl::Uniforms<attributes::InterpolationUniform<typename Ps::Attribute>...>;
    using UniformValues = typename Uniforms::Values;

    UniformValues uniformValues(float currentZoom) const {
        (void)currentZoom; // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56958
        return UniformValues {
            typename attributes::InterpolationUniform<typename Ps::Attribute>::Value {
                binders.template get<Ps>()->interpolationFactor(currentZoom)
            }...
        };
    }

private:
    Binders binders;
};

} // namespace style
} // namespace mbgl
