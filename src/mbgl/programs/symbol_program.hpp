#pragma once

#include <mbgl/gl/context.hpp>
#include <mbgl/gl/program.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/interpolate.hpp>

#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/shaders/symbol_icon.hpp>
#include <mbgl/shaders/symbol_sdf.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>


#include <cmath>
#include <array>

namespace mbgl {

namespace style {
class SymbolPropertyValues;
} // namespace style

class RenderTile;
class TransformState;

namespace uniforms {
MBGL_DEFINE_UNIFORM_MATRIX(double, 4, u_gl_coord_matrix);
MBGL_DEFINE_UNIFORM_MATRIX(double, 4, u_label_plane_matrix);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_texture);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_halo);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gamma_scale);

MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_text);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_size_zoom_constant);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_size_feature_constant);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_size_t);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_size);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_rotate_symbol);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_aspect_ratio);
} // namespace uniforms

struct SymbolLayoutAttributes : gl::Attributes<
    attributes::a_pos_offset,
    attributes::a_data<uint16_t, 4>>
{
    static Vertex vertex(Point<float> labelAnchor,
                         Point<float> o,
                         float glyphOffsetY,
                         uint16_t tx,
                         uint16_t ty,
                         const Range<float>& sizeData) {
        return Vertex {
            // combining pos and offset to reduce number of vertex attributes passed to shader (8 max for some devices)
            {{
                static_cast<int16_t>(labelAnchor.x),
                static_cast<int16_t>(labelAnchor.y),
                static_cast<int16_t>(::round(o.x * 32)),  // use 1/32 pixels for placement
                static_cast<int16_t>(::round((o.y + glyphOffsetY) * 32))
            }},
            {{
                tx,
                ty,
                static_cast<uint16_t>(sizeData.min * 256),
                static_cast<uint16_t>(sizeData.max * 256)
            }}
        };
    }
};

struct SymbolDynamicLayoutAttributes : gl::Attributes<attributes::a_projected_pos> {
    static Vertex vertex(Point<float> anchorPoint, float labelAngle) {
        return Vertex {
            {{
                 anchorPoint.x,
                 anchorPoint.y,
                 labelAngle
             }}
        };
    }
};

struct SymbolOpacityAttributes : gl::Attributes<attributes::a_fade_opacity> {
    static Vertex vertex(bool placed, float opacity) {
        return Vertex {
            {{ static_cast<uint8_t>((static_cast<uint8_t>(opacity * 127) << 1) | static_cast<uint8_t>(placed)) }}
        };
    }
};

struct ZoomEvaluatedSize {
    bool isZoomConstant;
    bool isFeatureConstant;
    float sizeT;
    float size;
    float layoutSize;
};
// Mimic the PaintPropertyBinder technique specifically for the {text,icon}-size layout properties
// in order to provide a 'custom' scheme for encoding the necessary attribute data.  As with
// PaintPropertyBinder, SymbolSizeBinder is an abstract class whose implementations handle the
// particular attribute & uniform logic needed by each possible type of the {Text,Icon}Size properties.
class SymbolSizeBinder {
public:
    virtual ~SymbolSizeBinder() = default;

    using Uniforms = gl::Uniforms<
        uniforms::u_is_size_zoom_constant,
        uniforms::u_is_size_feature_constant,
        uniforms::u_size_t,
        uniforms::u_size>;
    using UniformValues = Uniforms::Values;

    static std::unique_ptr<SymbolSizeBinder> create(const float tileZoom,
                                                    const style::PropertyValue<float>& sizeProperty,
                                                    const float defaultValue);

    virtual Range<float> getVertexSizeData(const GeometryTileFeature& feature) = 0;
    virtual ZoomEvaluatedSize evaluateForZoom(float currentZoom) const = 0;

    UniformValues uniformValues(float currentZoom) const {
        const ZoomEvaluatedSize u = evaluateForZoom(currentZoom);
        return UniformValues {
            uniforms::u_is_size_zoom_constant::Value( u.isZoomConstant ),
            uniforms::u_is_size_feature_constant::Value( u.isFeatureConstant),
            uniforms::u_size_t::Value( u.sizeT ),
            uniforms::u_size::Value( u.size )
        };
    }
};


class ConstantSymbolSizeBinder final : public SymbolSizeBinder {
public:
    ConstantSymbolSizeBinder(const float /*tileZoom*/, const float& size, const float /*defaultValue*/)
      : layoutSize(size) {}

    ConstantSymbolSizeBinder(const float /*tileZoom*/, const style::Undefined&, const float defaultValue)
      : layoutSize(defaultValue) {}

    ConstantSymbolSizeBinder(const float tileZoom, const style::PropertyExpression<float>& expression_, const float /*defaultValue*/)
      : layoutSize(expression_.evaluate(tileZoom + 1)),
        expression(expression_) {
        const Range<float> zoomLevels = expression_.getCoveringStops(tileZoom, tileZoom + 1);
        coveringRanges = std::make_tuple(
            zoomLevels,
            Range<float> { expression_.evaluate(zoomLevels.min), expression_.evaluate(zoomLevels.max) }
        );
    }

    Range<float> getVertexSizeData(const GeometryTileFeature&) override { return { 0.0f, 0.0f }; };

    ZoomEvaluatedSize evaluateForZoom(float currentZoom) const override {
        float size = layoutSize;
        bool isZoomConstant = !(coveringRanges || expression);
        if (coveringRanges) {
            // Even though we could get the exact value of the camera function
            // at z = currentZoom, we intentionally do not: instead, we interpolate
            // between the camera function values at a pair of zoom stops covering
            // [tileZoom, tileZoom + 1] in order to be consistent with this
            // restriction on composite functions.
            const Range<float>& zoomLevels = std::get<0>(*coveringRanges);
            const Range<float>& sizeLevels = std::get<1>(*coveringRanges);
            float t = util::clamp(
                expression->interpolationFactor(zoomLevels, currentZoom),
                0.0f, 1.0f
            );
            size = sizeLevels.min + t * (sizeLevels.max - sizeLevels.min);
        } else if (expression) {
            size = expression->evaluate(currentZoom);
        }

        const float unused = 0.0f;
        return { isZoomConstant, true, unused, size, layoutSize };
    }

    float layoutSize;
    optional<std::tuple<Range<float>, Range<float>>> coveringRanges;
    optional<style::PropertyExpression<float>> expression;
};

class SourceFunctionSymbolSizeBinder final : public SymbolSizeBinder {
public:
    using Vertex = gl::detail::Vertex<gl::AttributeType<uint16_t, 1>>;
    using VertexVector = gl::VertexVector<Vertex>;
    using VertexBuffer = gl::VertexBuffer<Vertex>;

    SourceFunctionSymbolSizeBinder(const float /*tileZoom*/, style::PropertyExpression<float> expression_, const float defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(defaultValue_) {
    }

    Range<float> getVertexSizeData(const GeometryTileFeature& feature) override {
        const float size = expression.evaluate(feature, defaultValue);
        return { size, size };
    };

    ZoomEvaluatedSize evaluateForZoom(float) const override {
        const float unused = 0.0f;
        return { true, false, unused, unused, unused };
    }

    style::PropertyExpression<float> expression;
    const float defaultValue;
};

class CompositeFunctionSymbolSizeBinder final : public SymbolSizeBinder {
public:

    CompositeFunctionSymbolSizeBinder(const float tileZoom, style::PropertyExpression<float> expression_, const float defaultValue_)
        : expression(std::move(expression_)),
          defaultValue(defaultValue_),
          layoutZoom(tileZoom + 1),
          coveringZoomStops(expression.getCoveringStops(tileZoom, tileZoom + 1))
    {}

    Range<float> getVertexSizeData(const GeometryTileFeature& feature) override {
        return {
            expression.evaluate(coveringZoomStops.min, feature, defaultValue),
            expression.evaluate(coveringZoomStops.max, feature, defaultValue)
        };
    };

    ZoomEvaluatedSize evaluateForZoom(float currentZoom) const override {
        float sizeInterpolationT = util::clamp(
            expression.interpolationFactor(coveringZoomStops, currentZoom),
            0.0f, 1.0f
        );

        const float unused = 0.0f;
        return { false, false, sizeInterpolationT, unused, unused };
    }

    style::PropertyExpression<float> expression;
    const float defaultValue;
    float layoutZoom;
    Range<float> coveringZoomStops;
};


template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class PaintProps>
class SymbolProgram {
public:
    using LayoutAttributes = LayoutAttrs;
    using LayoutVertex = typename LayoutAttributes::Vertex;

    using LayoutAndSizeAttributes = gl::ConcatenateAttributes<LayoutAttributes, SymbolDynamicLayoutAttributes, SymbolOpacityAttributes>;

    using PaintProperties = PaintProps;
    using PaintPropertyBinders = typename PaintProperties::Binders;
    using PaintAttributes = typename PaintPropertyBinders::Attributes;
    using Attributes = gl::ConcatenateAttributes<LayoutAndSizeAttributes, PaintAttributes>;

    using UniformValues = typename Uniforms::Values;
    using SizeUniforms = typename SymbolSizeBinder::Uniforms;
    using PaintUniforms = typename PaintPropertyBinders::Uniforms;
    using AllUniforms = gl::ConcatenateUniforms<Uniforms, SizeUniforms, PaintUniforms>;

    using ProgramType = gl::Program<Primitive, Attributes, AllUniforms>;

    ProgramType program;

    SymbolProgram(gl::Context& context, const ProgramParameters& programParameters)
        : program(ProgramType::createProgram(
            context,
            programParameters,
            Shaders::name,
            Shaders::vertexSource,
            Shaders::fragmentSource)) {
    }

    static typename AllUniforms::Values computeAllUniformValues(
        const UniformValues& uniformValues,
        const SymbolSizeBinder& symbolSizeBinder,
        const PaintPropertyBinders& paintPropertyBinders,
        const typename PaintProperties::PossiblyEvaluated& currentProperties,
        float currentZoom) {
        return uniformValues.concat(symbolSizeBinder.uniformValues(currentZoom))
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));
    }

    static typename Attributes::Bindings computeAllAttributeBindings(
        const gl::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
        const gl::VertexBuffer<SymbolDynamicLayoutAttributes::Vertex>& dynamicLayoutVertexBuffer,
        const gl::VertexBuffer<SymbolOpacityAttributes::Vertex>& opacityVertexBuffer,
        const PaintPropertyBinders& paintPropertyBinders,
        const typename PaintProperties::PossiblyEvaluated& currentProperties) {
        assert(layoutVertexBuffer.vertexCount == dynamicLayoutVertexBuffer.vertexCount &&
               layoutVertexBuffer.vertexCount == opacityVertexBuffer.vertexCount);
        return LayoutAttributes::bindings(layoutVertexBuffer)
            .concat(SymbolDynamicLayoutAttributes::bindings(dynamicLayoutVertexBuffer))
            .concat(SymbolOpacityAttributes::bindings(opacityVertexBuffer))
            .concat(paintPropertyBinders.attributeBindings(currentProperties));
    }

    static uint32_t activeBindingCount(const typename Attributes::Bindings& allAttributeBindings) {
        return Attributes::activeBindingCount(allAttributeBindings);
    }

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const SegmentVector<Attributes>& segments,
              const typename AllUniforms::Values& allUniformValues,
              const typename Attributes::Bindings& allAttributeBindings,
              const std::string& layerID) {
        for (auto& segment : segments) {
            auto vertexArrayIt = segment.vertexArrays.find(layerID);

            if (vertexArrayIt == segment.vertexArrays.end()) {
                vertexArrayIt = segment.vertexArrays.emplace(layerID, context.createVertexArray()).first;
            }

            program.draw(
                context,
                std::move(drawMode),
                std::move(depthMode),
                std::move(stencilMode),
                std::move(colorMode),
                allUniformValues,
                vertexArrayIt->second,
                Attributes::offsetBindings(allAttributeBindings, segment.vertexOffset),
                indexBuffer,
                segment.indexOffset,
                segment.indexLength);
        }
    }
};

class SymbolIconProgram : public SymbolProgram<
    shaders::symbol_icon,
    gl::Triangle,
    SymbolLayoutAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_label_plane_matrix,
        uniforms::u_gl_coord_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_texsize,
        uniforms::u_texture,
        uniforms::u_fade_change,
        uniforms::u_is_text,
        uniforms::u_camera_to_center_distance,
        uniforms::u_pitch,
        uniforms::u_pitch_with_map,
        uniforms::u_rotate_symbol,
        uniforms::u_aspect_ratio>,
    style::IconPaintProperties>
{
public:
    using SymbolProgram::SymbolProgram;

    static UniformValues uniformValues(const bool isText,
                                       const style::SymbolPropertyValues&,
                                       const Size& texsize,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const bool alongLine,
                                       const RenderTile&,
                                       const TransformState&,
                                       const float symbolFadeChange);
};

enum class SymbolSDFPart {
    Fill = 1,
    Halo = 0
};

template <class PaintProperties>
class SymbolSDFProgram : public SymbolProgram<
    shaders::symbol_sdf,
    gl::Triangle,
    SymbolLayoutAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_label_plane_matrix,
        uniforms::u_gl_coord_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_texsize,
        uniforms::u_texture,
        uniforms::u_fade_change,
        uniforms::u_is_text,
        uniforms::u_camera_to_center_distance,
        uniforms::u_pitch,
        uniforms::u_pitch_with_map,
        uniforms::u_rotate_symbol,
        uniforms::u_aspect_ratio,
        uniforms::u_gamma_scale,
        uniforms::u_is_halo>,
    PaintProperties>
{
public:
    using BaseProgram = SymbolProgram<shaders::symbol_sdf,
        gl::Triangle,
        SymbolLayoutAttributes,
        gl::Uniforms<
            uniforms::u_matrix,
            uniforms::u_label_plane_matrix,
            uniforms::u_gl_coord_matrix,
            uniforms::u_extrude_scale,
            uniforms::u_texsize,
            uniforms::u_texture,
            uniforms::u_fade_change,
            uniforms::u_is_text,
            uniforms::u_camera_to_center_distance,
            uniforms::u_pitch,
            uniforms::u_pitch_with_map,
            uniforms::u_rotate_symbol,
            uniforms::u_aspect_ratio,
            uniforms::u_gamma_scale,
            uniforms::u_is_halo>,
        PaintProperties>;

    using UniformValues = typename BaseProgram::UniformValues;



    using BaseProgram::BaseProgram;

    static UniformValues uniformValues(const bool isText,
                                       const style::SymbolPropertyValues&,
                                       const Size& texsize,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const bool alongLine,
                                       const RenderTile&,
                                       const TransformState&,
                                       const float SymbolFadeChange,
                                       const SymbolSDFPart);
};

using SymbolSDFIconProgram = SymbolSDFProgram<style::IconPaintProperties>;
using SymbolSDFTextProgram = SymbolSDFProgram<style::TextPaintProperties>;

using SymbolLayoutVertex = SymbolLayoutAttributes::Vertex;
using SymbolIconAttributes = SymbolIconProgram::Attributes;
using SymbolTextAttributes = SymbolSDFTextProgram::Attributes;

} // namespace mbgl
