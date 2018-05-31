#pragma once

#include <mbgl/programs/attributes.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/type_list.hpp>
#include <mbgl/renderer/possibly_evaluated_property_value.hpp>
#include <mbgl/renderer/paint_property_statistics.hpp>

#include <bitset>

namespace mbgl {

/*
   ZoomInterpolatedAttribute<Attr> is a 'compound' attribute, representing two values of the
   the base attribute Attr.  These two values are provided to the shader to allow interpolation
   between zoom levels, without the need to repopulate vertex buffers each frame as the map is
   being zoomed.
*/
template <class A>
using ZoomInterpolatedAttributeType = gl::Attribute<typename A::ValueType, A::Dimensions * 2>;

inline std::array<float, 1> attributeValue(float v) {
    return {{ v }};
}

/*
    Encode a four-component color value into a pair of floats.  Since csscolorparser
    uses 8-bit precision for each color component, for each float we use the upper 8
    bits for one component (e.g. (color.r * 255) * 256), and the lower 8 for another.
    
    Also note that colors come in as floats 0..1, so we scale by 255.
*/
inline std::array<float, 2> attributeValue(const Color& color) {
    return {{
        static_cast<float>(mbgl::attributes::packUint8Pair(255 * color.r, 255 * color.g)),
        static_cast<float>(mbgl::attributes::packUint8Pair(255 * color.b, 255 * color.a))
    }};
}

template <size_t N>
std::array<float, N*2> zoomInterpolatedAttributeValue(const std::array<float, N>& min, const std::array<float, N>& max) {
    std::array<float, N*2> result;
    for (size_t i = 0; i < N; i++) {
        result[i]   = min[i];
        result[i+N] = max[i];
    }
    return result;
}

/*
   PaintPropertyBinder is an abstract class serving as the interface definition for
   the strategy used for constructing, uploading, and binding paint property data as
   GLSL attributes.

   It has three concrete subclasses, one for each of the three strategies we use:

   * For _constant_ properties -- those whose value is a constant, or the constant
     result of evaluating a camera function at a particular camera position -- we
     don't need a vertex buffer, and instead use a uniform.
   * For source functions, we use a vertex buffer with a single attribute value,
     the evaluated result of the source function for the given feature.
   * For composite functions, we use a vertex buffer with two attributes: min and
     max values covering the range of zooms at which we expect the tile to be
     displayed. These values are calculated by evaluating the composite function for
     the given feature at strategically chosen zoom levels. In addition to this
     attribute data, we also use a uniform value which the shader uses to interpolate
     between the min and max value at the final displayed zoom level. The use of a
     uniform allows us to cheaply update the value on every frame.

   Note that the shader source varies depending on whether we're using a uniform or
   attribute. Like GL JS, we dynamically compile shaders at runtime to accomodate this.
*/
template <class T, class A>
class PaintPropertyBinder {
public:

    virtual ~PaintPropertyBinder() = default;

    virtual void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) = 0;
    virtual void upload(gl::Context& context) = 0;
    virtual optional<gl::AttributeBinding> attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const = 0;
    virtual float interpolationFactor(float currentZoom) const = 0;
    virtual T uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const = 0;

    static std::unique_ptr<PaintPropertyBinder> create(const PossiblyEvaluatedPropertyValue<T>& value, float zoom, T defaultValue);

    PaintPropertyStatistics<T> statistics;
};

template <class T, class A>
class ConstantPaintPropertyBinder : public PaintPropertyBinder<T, A> {
public:
    ConstantPaintPropertyBinder(T constant_)
        : constant(std::move(constant_)) {
    }

    void populateVertexVector(const GeometryTileFeature&, std::size_t) override {}
    void upload(gl::Context&) override {}

    optional<gl::AttributeBinding> attributeBinding(const PossiblyEvaluatedPropertyValue<T>&) const override {
        return {};
    }

    float interpolationFactor(float) const override {
        return 0.0f;
    }

    T uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        return currentValue.constantOr(constant);
    }

private:
    T constant;
};

template <class T, class A>
class SourceFunctionPaintPropertyBinder : public PaintPropertyBinder<T, A> {
public:
    using BaseAttribute = A;
    using BaseAttributeValue = typename BaseAttribute::Value;
    using BaseVertex = gl::detail::Vertex<BaseAttribute>;

    using Attribute = ZoomInterpolatedAttributeType<A>;

    SourceFunctionPaintPropertyBinder(style::PropertyExpression<T> expression_, T defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) override {
        auto evaluated = expression.evaluate(feature, defaultValue);
        this->statistics.add(evaluated);
        auto value = attributeValue(evaluated);
        for (std::size_t i = vertexVector.vertexSize(); i < length; ++i) {
            vertexVector.emplace_back(BaseVertex { value });
        }
    }

    void upload(gl::Context& context) override {
        vertexBuffer = context.createVertexBuffer(std::move(vertexVector));
    }

    optional<gl::AttributeBinding> attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return {};
        } else {
            return Attribute::binding(*vertexBuffer, 0, BaseAttribute::Dimensions);
        }
    }

    float interpolationFactor(float) const override {
        return 0.0f;
    }

    T uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return *currentValue.constant();
        } else {
            // Uniform values for vertex attribute arrays are unused.
            return {};
        }
    }

private:
    style::PropertyExpression<T> expression;
    T defaultValue;
    gl::VertexVector<BaseVertex> vertexVector;
    optional<gl::VertexBuffer<BaseVertex>> vertexBuffer;
};

template <class T, class A>
class CompositeFunctionPaintPropertyBinder : public PaintPropertyBinder<T, A> {
public:

    using Attribute = ZoomInterpolatedAttributeType<A>;
    using AttributeValue = typename Attribute::Value;
    using Vertex = gl::detail::Vertex<Attribute>;

    CompositeFunctionPaintPropertyBinder(style::PropertyExpression<T> expression_, float zoom, T defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)),
          zoomRange({zoom, zoom + 1}) {
    }

    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length) override {
        Range<T> range = expression.evaluate(zoomRange, feature, defaultValue);
        this->statistics.add(range.min);
        this->statistics.add(range.max);
        AttributeValue value = zoomInterpolatedAttributeValue(
            attributeValue(range.min),
            attributeValue(range.max));
        for (std::size_t i = vertexVector.vertexSize(); i < length; ++i) {
            vertexVector.emplace_back(Vertex { value });
        }
    }

    void upload(gl::Context& context) override {
        vertexBuffer = context.createVertexBuffer(std::move(vertexVector));
    }

    optional<gl::AttributeBinding> attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return {};
        } else {
            return Attribute::binding(*vertexBuffer, 0);
        }
    }

    float interpolationFactor(float currentZoom) const override {
        if (expression.useIntegerZoom) {
            return expression.interpolationFactor(zoomRange, std::floor(currentZoom));
        } else {
            return expression.interpolationFactor(zoomRange, currentZoom);
        }
    }

    T uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return *currentValue.constant();
        } else {
            // Uniform values for vertex attribute arrays are unused.
            return {};
        }
    }

private:
    style::PropertyExpression<T> expression;
    T defaultValue;
    Range<float> zoomRange;
    gl::VertexVector<Vertex> vertexVector;
    optional<gl::VertexBuffer<Vertex>> vertexBuffer;
};

template <class T, class A>
std::unique_ptr<PaintPropertyBinder<T, A>>
PaintPropertyBinder<T, A>::create(const PossiblyEvaluatedPropertyValue<T>& value, float zoom, T defaultValue) {
    return value.match(
        [&] (const T& constant) -> std::unique_ptr<PaintPropertyBinder<T, A>> {
            return std::make_unique<ConstantPaintPropertyBinder<T, A>>(constant);
        },
        [&] (const style::PropertyExpression<T>& expression) -> std::unique_ptr<PaintPropertyBinder<T, A>> {
            if (expression.isZoomConstant()) {
                return std::make_unique<SourceFunctionPaintPropertyBinder<T, A>>(expression, defaultValue);
            } else {
                return std::make_unique<CompositeFunctionPaintPropertyBinder<T, A>>(expression, zoom, defaultValue);
            }
        }
    );
}

template <class Attr>
struct ZoomInterpolatedAttribute {
    static auto name() { return Attr::name(); }
    using Type = ZoomInterpolatedAttributeType<typename Attr::Type>;
};

template <class Attr>
struct InterpolationUniform : gl::UniformScalar<InterpolationUniform<Attr>, float> {
    static auto name() {
        static const std::string name = Attr::name() + std::string("_t");
        return name.c_str();
    }
};

template <class Ps>
class PaintPropertyBinders;

template <class... Ps>
class PaintPropertyBinders<TypeList<Ps...>> {
public:
    template <class P>
    using Binder = PaintPropertyBinder<typename P::Type, typename P::Attribute::Type>;

    using Binders = IndexedTuple<
        TypeList<Ps...>,
        TypeList<std::unique_ptr<Binder<Ps>>...>>;

    template <class EvaluatedProperties>
    PaintPropertyBinders(const EvaluatedProperties& properties, float z)
        : binders(Binder<Ps>::create(properties.template get<Ps>(), z, Ps::defaultValue())...) {
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

    template <class P>
    using Attribute = ZoomInterpolatedAttribute<typename P::Attribute>;

    using Attributes = gl::Attributes<Attribute<Ps>...>;
    using AttributeBindings = typename Attributes::Bindings;

    template <class EvaluatedProperties>
    AttributeBindings attributeBindings(const EvaluatedProperties& currentProperties) const {
        return AttributeBindings {
            binders.template get<Ps>()->attributeBinding(currentProperties.template get<Ps>())...
        };
    }

    using Uniforms = gl::Uniforms<InterpolationUniform<typename Ps::Attribute>..., typename Ps::Uniform...>;
    using UniformValues = typename Uniforms::Values;

    template <class EvaluatedProperties>
    UniformValues uniformValues(float currentZoom, const EvaluatedProperties& currentProperties) const {
        (void)currentZoom; // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56958
        return UniformValues {
            typename InterpolationUniform<typename Ps::Attribute>::Value {
                binders.template get<Ps>()->interpolationFactor(currentZoom)
            }...,
            typename Ps::Uniform::Value {
                binders.template get<Ps>()->uniformValue(currentProperties.template get<Ps>())
            }...
        };
    }

    template <class P>
    const auto& statistics() const {
        return binders.template get<P>()->statistics;
    }


    using Bitset = std::bitset<sizeof...(Ps)>;

    template <class EvaluatedProperties>
    static Bitset constants(const EvaluatedProperties& currentProperties) {
        Bitset result;
        util::ignore({
            result.set(TypeIndex<Ps, Ps...>::value,
                       currentProperties.template get<Ps>().isConstant())...
        });
        return result;
    }

    template <class EvaluatedProperties>
    static std::vector<std::string> defines(const EvaluatedProperties& currentProperties) {
        std::vector<std::string> result;
        util::ignore({
            (result.push_back(currentProperties.template get<Ps>().isConstant()
                ? std::string("#define HAS_UNIFORM_") + Ps::Uniform::name()
                : std::string()), 0)...
        });
        return result;
    }

private:
    Binders binders;
};

} // namespace mbgl
