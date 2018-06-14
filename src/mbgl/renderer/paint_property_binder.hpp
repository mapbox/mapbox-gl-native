#pragma once

#include <mbgl/programs/attributes.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/type_list.hpp>
#include <mbgl/renderer/possibly_evaluated_property_value.hpp>
#include <mbgl/renderer/paint_property_statistics.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/layout/pattern_layout.hpp>

#include <bitset>

namespace mbgl {

/*
   ZoomInterpolatedAttribute<Attr> is a 'compound' attribute, representing two values of the
   the base attribute Attr.  These two values are provided to the shader to allow interpolation
   between zoom levels, without the need to repopulate vertex buffers each frame as the map is
   being zoomed.
*/
template <class A>
using ZoomInterpolatedAttributeType = gl::AttributeType<typename A::ValueType, A::Dimensions * 2>;

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

template <class T, class UniformValueType, class PossiblyEvaluatedType, class... As>
class PaintPropertyBinder {
public:

    virtual ~PaintPropertyBinder() = default;

    virtual void populateVertexVector(const GeometryTileFeature& feature, std::size_t length, const ImagePositions&, const optional<PatternDependency>&) = 0;
    virtual void upload(gl::Context& context) = 0;
    virtual void setPatternParameters(const optional<ImagePosition>&, const optional<ImagePosition>&, CrossfadeParameters&) = 0;
    virtual std::tuple<ExpandToType<As, optional<gl::AttributeBinding>>...> attributeBinding(const PossiblyEvaluatedType& currentValue) const = 0;
    virtual std::tuple<ExpandToType<As, float>...> interpolationFactor(float currentZoom) const = 0;
    virtual std::tuple<ExpandToType<As, UniformValueType>...> uniformValue(const PossiblyEvaluatedType& currentValue) const = 0;

    static std::unique_ptr<PaintPropertyBinder> create(const PossiblyEvaluatedType& value, float zoom, T defaultValue);

    PaintPropertyStatistics<T> statistics;
};

template <class T, class A>
class ConstantPaintPropertyBinder : public PaintPropertyBinder<T, T, PossiblyEvaluatedPropertyValue<T>, A> {
public:
    ConstantPaintPropertyBinder(T constant_)
        : constant(std::move(constant_)) {
    }

    void populateVertexVector(const GeometryTileFeature&, std::size_t, const ImagePositions&, const optional<PatternDependency>&) override {}
    void upload(gl::Context&) override {}
    void setPatternParameters(const optional<ImagePosition>&, const optional<ImagePosition>&, CrossfadeParameters&) override {};

    std::tuple<optional<gl::AttributeBinding>> attributeBinding(const PossiblyEvaluatedPropertyValue<T>&) const override {
        return std::tuple<optional<gl::AttributeBinding>> {};
    }

    std::tuple<float> interpolationFactor(float) const override {
        return std::tuple<float> { 0.0f };
    }

    std::tuple<T> uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        return std::tuple<T> { currentValue.constantOr(constant) };
    }

private:
    T constant;
};

template <class T, class... As>
class ConstantCrossFadedPaintPropertyBinder : public PaintPropertyBinder<T, std::array<uint16_t, 4>,PossiblyEvaluatedPropertyValue<Faded<T>>, As...> {
public:
    ConstantCrossFadedPaintPropertyBinder(Faded<T> constant_)
        : constant(std::move(constant_)), constantPatternPositions({}) {
    }

    void populateVertexVector(const GeometryTileFeature&, std::size_t, const ImagePositions&, const optional<PatternDependency>&) override {}
    void upload(gl::Context&) override {}

    void setPatternParameters(const optional<ImagePosition>& posA, const optional<ImagePosition>& posB, CrossfadeParameters&) override {
        if (!posA && !posB) {
            return;
        } else {
            constantPatternPositions = std::tuple<std::array<uint16_t, 4>, std::array<uint16_t, 4>> { posB->tlbr(), posA->tlbr() };
        }
    }

    std::tuple<optional<gl::AttributeBinding>, optional<gl::AttributeBinding>>
    attributeBinding(const PossiblyEvaluatedPropertyValue<Faded<T>>&) const override {
        return std::tuple<optional<gl::AttributeBinding>, optional<gl::AttributeBinding>> {};
    }

    std::tuple<float, float> interpolationFactor(float) const override {
        return std::tuple<float, float> { 0.0f, 0.0f };
    }

    std::tuple<std::array<uint16_t, 4>, std::array<uint16_t, 4>> uniformValue(const PossiblyEvaluatedPropertyValue<Faded<T>>&) const override {
        return constantPatternPositions;
    }

private:
    Faded<T> constant;
    std::tuple<std::array<uint16_t, 4>, std::array<uint16_t, 4>> constantPatternPositions;
};

template <class T, class A>
class SourceFunctionPaintPropertyBinder : public PaintPropertyBinder<T, T, PossiblyEvaluatedPropertyValue<T>, A> {
public:
    using BaseAttribute = A;
    using BaseVertex = gl::detail::Vertex<BaseAttribute>;

    using AttributeType = ZoomInterpolatedAttributeType<A>;

    SourceFunctionPaintPropertyBinder(style::PropertyExpression<T> expression_, T defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)) {
    }
    void setPatternParameters(const optional<ImagePosition>&, const optional<ImagePosition>&, CrossfadeParameters&) override {};
    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length, const ImagePositions&, const optional<PatternDependency>&) override {
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

    std::tuple<optional<gl::AttributeBinding>> attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return {};
        } else {
            return std::tuple<optional<gl::AttributeBinding>> { AttributeType::binding(*vertexBuffer, 0, BaseAttribute::Dimensions) };
        }
    }

    std::tuple<float> interpolationFactor(float) const override {
        return std::tuple<float> { 0.0f };
    }

    std::tuple<T> uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return std::tuple<T>{ *currentValue.constant() };
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
class CompositeFunctionPaintPropertyBinder : public PaintPropertyBinder<T, T, PossiblyEvaluatedPropertyValue<T>, A> {
public:

    using AttributeType = ZoomInterpolatedAttributeType<A>;
    using AttributeValue = typename AttributeType::Value;
    using Vertex = gl::detail::Vertex<AttributeType>;

    CompositeFunctionPaintPropertyBinder(style::PropertyExpression<T> expression_, float zoom, T defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)),
          zoomRange({zoom, zoom + 1}) {
    }
    void setPatternParameters(const optional<ImagePosition>&, const optional<ImagePosition>&, CrossfadeParameters&) override {};
    void populateVertexVector(const GeometryTileFeature& feature, std::size_t length, const ImagePositions&, const optional<PatternDependency>&) override {
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

    std::tuple<optional<gl::AttributeBinding>> attributeBinding(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return {};
        } else {
            return std::tuple<optional<gl::AttributeBinding>> { AttributeType::binding(*vertexBuffer, 0) };
        }
    }

    std::tuple<float> interpolationFactor(float currentZoom) const override {
        if (expression.useIntegerZoom) {
            return std::tuple<float> { expression.interpolationFactor(zoomRange, std::floor(currentZoom)) };
        } else {
            return std::tuple<float> { expression.interpolationFactor(zoomRange, currentZoom) };
        }
    }

    std::tuple<T> uniformValue(const PossiblyEvaluatedPropertyValue<T>& currentValue) const override {
        if (currentValue.isConstant()) {
            return std::tuple<T> { *currentValue.constant() };
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

template <class T, class A1, class A2>
class CompositeCrossFadedPaintPropertyBinder : public PaintPropertyBinder<T, std::array<uint16_t, 4>, PossiblyEvaluatedPropertyValue<Faded<T>>, A1, A2> {
public:
    using AttributeType = ZoomInterpolatedAttributeType<A1>;
    using AttributeType2 = ZoomInterpolatedAttributeType<A2>;

    using BaseAttribute = A1;
    using BaseAttributeValue = typename BaseAttribute::Value;

    using BaseAttribute2 = A2;
    using BaseAttributeValue2 = typename BaseAttribute2::Value;

    using Vertex = gl::detail::Vertex<BaseAttribute>;
    using Vertex2 = gl::detail::Vertex<BaseAttribute2>;

    CompositeCrossFadedPaintPropertyBinder(style::PropertyExpression<T> expression_, float zoom, T defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)),
          zoomRange({zoom, zoom + 1}) {
    }

    void setPatternParameters(const optional<ImagePosition>&, const optional<ImagePosition>&, CrossfadeParameters& crossfade_) override {
        crossfade = crossfade_;
    };

    void populateVertexVector(const GeometryTileFeature&, std::size_t length, const ImagePositions& patternPositions, const optional<PatternDependency>& patternDependencies) override {
        if (!patternDependencies) return;
        if (!patternPositions.empty()) {
            const auto min = patternPositions.find(patternDependencies->min);
            const auto mid = patternPositions.find(patternDependencies->mid);
            const auto max = patternPositions.find(patternDependencies->max);

            const auto end = patternPositions.end();
            if (min == end || mid == end || max == end) return;

            const ImagePosition imageMin = min->second;
            const ImagePosition imageMid = mid->second;
            const ImagePosition imageMax = max->second;

            for (std::size_t i = zoomInVertexVector.vertexSize(); i < length; ++i) {
                patternToVertexVector.emplace_back(Vertex { imageMid.tlbr() });
                zoomInVertexVector.emplace_back(Vertex2 { imageMin.tlbr() });
                zoomOutVertexVector.emplace_back(Vertex2 { imageMax.tlbr() });
            }
        }
    }

    void upload(gl::Context& context) override {
        patternToVertexBuffer = context.createVertexBuffer(std::move(patternToVertexVector));
        zoomInVertexBuffer = context.createVertexBuffer(std::move(zoomInVertexVector));
        zoomOutVertexBuffer = context.createVertexBuffer(std::move(zoomOutVertexVector));
    }

    std::tuple<optional<gl::AttributeBinding>, optional<gl::AttributeBinding>> attributeBinding(const PossiblyEvaluatedPropertyValue<Faded<T>>& currentValue) const override {
        if (currentValue.isConstant()) {
            return {};
        } else {
            return std::tuple<optional<gl::AttributeBinding>, optional<gl::AttributeBinding>> {
                AttributeType::binding(*patternToVertexBuffer, 0, BaseAttribute::Dimensions),
                AttributeType2::binding(
                    crossfade.fromScale == 2 ? *zoomInVertexBuffer : *zoomOutVertexBuffer,
                    0, BaseAttribute2::Dimensions) };
        }
    }

    std::tuple<float, float> interpolationFactor(float) const override {
        return std::tuple<float, float> { 0.0f, 0.0f };
    }

    std::tuple<std::array<uint16_t, 4>, std::array<uint16_t, 4>>  uniformValue(const PossiblyEvaluatedPropertyValue<Faded<T>>& ) const override {
        // Uniform values for vertex attribute arrays are unused.
        return {};
    }

private:
    style::PropertyExpression<T> expression;
    T defaultValue;
    Range<float> zoomRange;
    gl::VertexVector<Vertex> patternToVertexVector;
    gl::VertexVector<Vertex2> zoomInVertexVector;
    gl::VertexVector<Vertex2> zoomOutVertexVector;
    optional<gl::VertexBuffer<Vertex>> patternToVertexBuffer;
    optional<gl::VertexBuffer<Vertex2>> zoomInVertexBuffer;
    optional<gl::VertexBuffer<Vertex2>> zoomOutVertexBuffer;
    CrossfadeParameters crossfade;
};

template <class T, class PossiblyEvaluatedType>
struct CreateBinder {
    template <class A>
    static std::unique_ptr<PaintPropertyBinder<T, T, PossiblyEvaluatedType, A>> create(const PossiblyEvaluatedType& value, float zoom, T defaultValue) {
        return value.match(
            [&] (const T& constant) -> std::unique_ptr<PaintPropertyBinder<T, T, PossiblyEvaluatedType, A>> {
                return std::make_unique<ConstantPaintPropertyBinder<T, A>>(constant);
            },
            [&] (const style::PropertyExpression<T>& expression) -> std::unique_ptr<PaintPropertyBinder<T, T, PossiblyEvaluatedType, A>>  {
                if (expression.isZoomConstant()) {
                    return std::make_unique<SourceFunctionPaintPropertyBinder<T, A>>(expression, defaultValue);
                } else {
                    return std::make_unique<CompositeFunctionPaintPropertyBinder<T, A>>(expression, zoom, defaultValue);
                }
            }
        );
    }
};

template <class T>
struct CreateBinder<T, PossiblyEvaluatedPropertyValue<Faded<T>>> {
    template <class A1, class A2>
    static std::unique_ptr<PaintPropertyBinder<T, std::array<uint16_t, 4>, PossiblyEvaluatedPropertyValue<Faded<T>>, A1, A2>> create(const PossiblyEvaluatedPropertyValue<Faded<T>>& value, float zoom, T defaultValue) {
        return value.match(
            [&] (const Faded<T>& constant) -> std::unique_ptr<PaintPropertyBinder<T, std::array<uint16_t, 4>, PossiblyEvaluatedPropertyValue<Faded<T>>, A1, A2>> {
                return std::make_unique<ConstantCrossFadedPaintPropertyBinder<T, A1, A2>>(constant);
            },
            [&] (const style::PropertyExpression<T>& expression) -> std::unique_ptr<PaintPropertyBinder<T, std::array<uint16_t, 4>, PossiblyEvaluatedPropertyValue<Faded<T>>, A1, A2>>  {
                return std::make_unique<CompositeCrossFadedPaintPropertyBinder<T, A1, A2>>(expression, zoom, defaultValue);
            }
        );
    }
};

template <class T, class UniformValueType, class PossiblyEvaluatedType, class... As>
std::unique_ptr<PaintPropertyBinder<T, UniformValueType, PossiblyEvaluatedType, As... >>
PaintPropertyBinder<T, UniformValueType, PossiblyEvaluatedType, As...>::create(const PossiblyEvaluatedType& value, float zoom, T defaultValue) {
    return CreateBinder<T, PossiblyEvaluatedType>::template create<As...>(value, zoom, defaultValue);
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
private:
    template <class T, class PossiblyEvaluatedType, class... As>
    struct Detail;

    template <class T, class UniformValueType, class PossiblyEvaluatedType, class... As>
    struct Detail<T, UniformValueType, PossiblyEvaluatedType, TypeList<As...>> {
        using Binder = PaintPropertyBinder<T, UniformValueType, PossiblyEvaluatedType, typename As::Type...>;
        using ZoomInterpolatedAttributeList = TypeList<ZoomInterpolatedAttribute<As>...>;
        using InterpolationUniformList = TypeList<InterpolationUniform<As>...>;
    };

    template <class P>
    using Property = Detail<typename P::Type, typename P::Uniform::Type, typename P::PossiblyEvaluatedType, typename P::Attributes>;

public:
    template <class P>
    using Binder = typename Property<P>::Binder;

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

    void populateVertexVectors(const GeometryTileFeature& feature, std::size_t length, const ImagePositions& patternPositions, const optional<PatternDependency>& patternDependencies) {
        util::ignore({
            (binders.template get<Ps>()->populateVertexVector(feature, length, patternPositions, patternDependencies), 0)...
        });
    }

    void setPatternParameters(const optional<ImagePosition>& posA, const optional<ImagePosition>& posB, CrossfadeParameters& crossfade) const {
        util::ignore({
            (binders.template get<Ps>()->setPatternParameters(posA, posB, crossfade), 0)...
        });
    }

    void upload(gl::Context& context) {
        util::ignore({
            (binders.template get<Ps>()->upload(context), 0)...
        });
    }

    template <class P>
    using ZoomInterpolatedAttributeList = typename Property<P>::ZoomInterpolatedAttributeList;
    template <class P>
    using InterpolationUniformList = typename Property<P>::InterpolationUniformList;

    using Attributes = typename TypeListConcat<ZoomInterpolatedAttributeList<Ps>...>::template ExpandInto<gl::Attributes>;
    using AttributeBindings = typename Attributes::Bindings;

    template <class EvaluatedProperties>
    AttributeBindings attributeBindings(const EvaluatedProperties& currentProperties) const {
        return AttributeBindings { std::tuple_cat(
           binders.template get<Ps>()->attributeBinding(currentProperties.template get<Ps>())...
        ) };
    }

    using Uniforms = typename TypeListConcat<InterpolationUniformList<Ps>..., typename Ps::Uniforms...>::template ExpandInto<gl::Uniforms>;
    using UniformValues = typename Uniforms::Values;

    template <class EvaluatedProperties>
    UniformValues uniformValues(float currentZoom, EvaluatedProperties& currentProperties) const {
        (void)currentZoom; // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56958
        return UniformValues (
            std::tuple_cat(
                // interpolation uniform values
                binders.template get<Ps>()->interpolationFactor(currentZoom)...,
                // uniform values
                binders.template get<Ps>()->uniformValue(currentProperties.template get<Ps>())...)
        );
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

    template <class>
    struct UniformDefines;

    template <class... Us>
    struct UniformDefines<TypeList<Us...>> {
        static void appendDefines(std::vector<std::string>& defines) {
            util::ignore({
                (defines.push_back(std::string("#define HAS_UNIFORM_") + Us::name()), 0)...
            });
        }
    };

    template <class EvaluatedProperties>
    static std::vector<std::string> defines(const EvaluatedProperties& currentProperties) {
        std::vector<std::string> result;
        util::ignore({
            (currentProperties.template get<Ps>().isConstant()
                ? UniformDefines<typename Ps::Uniforms>::appendDefines(result)
                : (void) 0, 0)...
        });
        return result;
    }

private:
    Binders binders;
};

} // namespace mbgl
