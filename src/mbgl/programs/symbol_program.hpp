#pragma once

#include <mbgl/gl/context.hpp>
#include <mbgl/gl/program.hpp>
#include <mbgl/gl/features.hpp>
#include <mbgl/programs/binary_program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/shaders/shaders.hpp>
#include <mbgl/util/io.hpp>


#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/symbol_icon.hpp>
#include <mbgl/shaders/symbol_sdf.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>


#include <cmath>
#include <array>

namespace mbgl {

namespace style {
class SymbolPropertyValues;
} // namespace style

class RenderTile;
class TransformState;

namespace uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_texsize);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_rotate_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_pitch_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_texture);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_fadetexture);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_aspect_ratio);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_halo);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gamma_scale);

MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_text);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_zoom_constant);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_is_feature_constant);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_size_t);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_size);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_layout_size);
} // namespace uniforms

struct SymbolLayoutAttributes : gl::Attributes<
    attributes::a_pos_offset,
    attributes::a_data<uint16_t, 4>>
{
    static Vertex vertex(Point<float> a,
                         Point<float> o,
                         uint16_t tx,
                         uint16_t ty,
                         float minzoom,
                         float maxzoom,
                         float labelminzoom,
                         uint8_t labelangle) {
        return Vertex {
            // combining pos and offset to reduce number of vertex attributes passed to shader (8 max for some devices)
            {{
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y),
                static_cast<int16_t>(::round(o.x * 64)),  // use 1/64 pixels for placement
                static_cast<int16_t>(::round(o.y * 64))
            }},
            {{
                static_cast<uint16_t>(tx / 4),
                static_cast<uint16_t>(ty / 4),
                mbgl::attributes::packUint8Pair(
                   static_cast<uint8_t>(labelminzoom * 10), // 1/10 zoom levels: z16 == 160
                   static_cast<uint8_t>(labelangle)
                ),
                mbgl::attributes::packUint8Pair(
                   static_cast<uint8_t>(minzoom * 10),
                   static_cast<uint8_t>(::fmin(maxzoom, 25) * 10)
                )
            }}
        };
    }
};
    
struct SymbolSizeAttributes : gl::Attributes<attributes::a_size> {
    using SourceFunctionVertex = gl::detail::Vertex<gl::Attribute<uint16_t, 1>>;
    struct monostate {};
    using VertexVector = variant<
        monostate,
        gl::VertexVector<SourceFunctionVertex>,
        gl::VertexVector<Vertex>>;
    
    using VertexBuffer = variant<
        monostate,
        gl::VertexBuffer<SourceFunctionVertex>,
        gl::VertexBuffer<Vertex>>;
    
    using Attribute = attributes::a_size::Type;
    
    static Bindings attributeBindings(const VertexBuffer& buffer) {
        return buffer.match(
                [&] (const monostate&) {
                    return Bindings { Attribute::ConstantBinding {{{0, 0, 0}}} };
                },
                [&] (const gl::VertexBuffer<SourceFunctionVertex>& buffer) {
                    return Bindings { Attribute::variableBinding(buffer, 0, 1) };
                },
                [&] (const gl::VertexBuffer<Vertex>& buffer) {
                    return Bindings { Attribute::variableBinding(buffer, 0) };
                }
            );
        };
};


class SymbolSizeData {
public:
    SymbolSizeData(float tileZoom, const style::DataDrivenPropertyValue<float>& size, float defaultSize_)
        : sizePropertyValue(size),
          layoutZoom(tileZoom + 1),
          defaultSize(defaultSize_) {
        size.match(
            [&] (float constantSize) { layoutSize = constantSize; },
            [&] (const style::CameraFunction<float>& fn) {
                layoutSize = fn.evaluate(layoutZoom);
                coveringZoomStops = fn.coveringZoomStops(tileZoom, tileZoom + 1);
            },
            [&] (const style::SourceFunction<float>&) {
                vertices = gl::VertexVector<SymbolSizeAttributes::SourceFunctionVertex> {};
            },
            [&] (const style::CompositeFunction<float>& fn) {
                coveringZoomStops = fn.coveringZoomStops(tileZoom, tileZoom + 1);
                vertices = gl::VertexVector<SymbolSizeAttributes::Vertex> {};
            },
            [&] (const auto&) {}
        );
    };
    
    
    void upload(gl::Context& context) {
        vertices.match(
            // TODO: what's the best way to avoid copying the vertexVector here?
            [&] (gl::VertexVector<SymbolSizeAttributes::SourceFunctionVertex> vertexVector) {
                // TODO - why does directly assigning vertexBuffer = context.createVertexBuffer(...) fail?
                vertexBuffer = SymbolSizeAttributes::VertexBuffer { context.createVertexBuffer(std::move(vertexVector)) };
            },
            [&] (gl::VertexVector<SymbolSizeAttributes::Vertex> vertexVector) {
                vertexBuffer = SymbolSizeAttributes::VertexBuffer { context.createVertexBuffer(std::move(vertexVector)) };
            },
            [&] (const auto&) {}
        );
    }
    
    style::DataDrivenPropertyValue<float> sizePropertyValue;
    float layoutZoom;
    float layoutSize;
    float defaultSize;
    optional<Range<float>> coveringZoomStops;
    SymbolSizeAttributes::VertexVector vertices;
    SymbolSizeAttributes::VertexBuffer vertexBuffer;
};


template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class PaintProperties>
class SymbolProgram {
public:
    using LayoutAttributes = LayoutAttrs;
    using LayoutVertex = typename LayoutAttributes::Vertex;
    
    using LayoutAndSizeAttributes = gl::ConcatenateAttributes<LayoutAttributes, SymbolSizeAttributes>;

    using PaintPropertyBinders = typename PaintProperties::Binders;
    using PaintAttributes = typename PaintPropertyBinders::Attributes;
    using Attributes = gl::ConcatenateAttributes<LayoutAndSizeAttributes, PaintAttributes>;

    using UniformValues = typename Uniforms::Values;
    using PaintUniforms = typename PaintPropertyBinders::Uniforms;
    using AllUniforms = gl::ConcatenateUniforms<Uniforms, PaintUniforms>;

    using ProgramType = gl::Program<Primitive, Attributes, AllUniforms>;

    ProgramType program;

    SymbolProgram(gl::Context& context, const ProgramParameters& programParameters)
        : program([&] {
#if MBGL_HAS_BINARY_PROGRAMS
              if (!programParameters.cacheDir.empty() && context.supportsProgramBinaries()) {
                  const std::string vertexSource =
                      shaders::vertexSource(programParameters, Shaders::vertexSource);
                  const std::string fragmentSource =
                      shaders::fragmentSource(programParameters, Shaders::fragmentSource);
                  const std::string cachePath =
                      shaders::programCachePath(programParameters, Shaders::name);
                  const std::string identifier =
                      shaders::programIdentifier(vertexSource, fragmentSource);

                  try {
                      if (auto cachedBinaryProgram = util::readFile(cachePath)) {
                          const BinaryProgram binaryProgram(std::move(*cachedBinaryProgram));
                          if (binaryProgram.identifier() == identifier) {
                              return ProgramType{ context, binaryProgram };
                          } else {
                              Log::Warning(Event::OpenGL,
                                           "Cached program %s changed. Recompilation required.",
                                           Shaders::name);
                          }
                      }
                  } catch (std::runtime_error& error) {
                      Log::Warning(Event::OpenGL, "Could not load cached program: %s",
                                   error.what());
                  }

                  // Compile the shader
                  ProgramType result{ context, vertexSource, fragmentSource };

                  try {
                      if (const auto binaryProgram =
                              result.template get<BinaryProgram>(context, identifier)) {
                          util::write_file(cachePath, binaryProgram->serialize());
                          Log::Warning(Event::OpenGL, "Caching program in: %s", cachePath.c_str());
                      }
                  } catch (std::runtime_error& error) {
                      Log::Warning(Event::OpenGL, "Failed to cache program: %s", error.what());
                  }

                  return std::move(result);
              }
#endif
              return ProgramType{
                  context, shaders::vertexSource(programParameters, Shaders::vertexSource),
                  shaders::fragmentSource(programParameters, Shaders::fragmentSource)
              };
          }()) {
    }

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              UniformValues&& uniformValues,
              const gl::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
              const SymbolSizeAttributes::VertexBuffer& sizeVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const gl::SegmentVector<Attributes>& segments,
              const PaintPropertyBinders& paintPropertyBinders,
              const typename PaintProperties::Evaluated& currentProperties,
              float currentZoom) {
        program.draw(
            context,
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            uniformValues
                .concat(paintPropertyBinders.uniformValues(currentZoom)),
            LayoutAttributes::allVariableBindings(layoutVertexBuffer)
                .concat(SymbolSizeAttributes::attributeBindings(sizeVertexBuffer))
                .concat(paintPropertyBinders.attributeBindings(currentProperties)),
            indexBuffer,
            segments
        );
    }
};

class SymbolIconProgram : public SymbolProgram<
    shaders::symbol_icon,
    gl::Triangle,
    SymbolLayoutAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_texsize,
        uniforms::u_zoom,
        uniforms::u_rotate_with_map,
        uniforms::u_texture,
        uniforms::u_fadetexture,
        uniforms::u_is_text,
        uniforms::u_is_zoom_constant,
        uniforms::u_is_feature_constant,
        uniforms::u_size_t,
        uniforms::u_size,
        uniforms::u_layout_size>,
    style::IconPaintProperties>
{
public:
    using SymbolProgram::SymbolProgram;

    static UniformValues uniformValues(const bool isText,
                                       const style::SymbolPropertyValues&,
                                       const SymbolSizeData& sizeData,
                                       const Size& texsize,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const RenderTile&,
                                       const TransformState&);
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
        uniforms::u_extrude_scale,
        uniforms::u_texsize,
        uniforms::u_zoom,
        uniforms::u_rotate_with_map,
        uniforms::u_texture,
        uniforms::u_fadetexture,
        uniforms::u_is_text,
        uniforms::u_is_zoom_constant,
        uniforms::u_is_feature_constant,
        uniforms::u_size_t,
        uniforms::u_size,
        uniforms::u_layout_size,
        uniforms::u_gamma_scale,
        uniforms::u_pitch,
        uniforms::u_bearing,
        uniforms::u_aspect_ratio,
        uniforms::u_pitch_with_map,
        uniforms::u_is_halo>,
    PaintProperties>
{
public:
    using BaseProgram = SymbolProgram<shaders::symbol_sdf,
        gl::Triangle,
        SymbolLayoutAttributes,
        gl::Uniforms<
            uniforms::u_matrix,
            uniforms::u_extrude_scale,
            uniforms::u_texsize,
            uniforms::u_zoom,
            uniforms::u_rotate_with_map,
            uniforms::u_texture,
            uniforms::u_fadetexture,
            uniforms::u_is_text,
            uniforms::u_is_zoom_constant,
            uniforms::u_is_feature_constant,
            uniforms::u_size_t,
            uniforms::u_size,
            uniforms::u_layout_size,
            uniforms::u_gamma_scale,
            uniforms::u_pitch,
            uniforms::u_bearing,
            uniforms::u_aspect_ratio,
            uniforms::u_pitch_with_map,
            uniforms::u_is_halo>,
        PaintProperties>;
    
    using UniformValues = typename BaseProgram::UniformValues;
    

    
    using BaseProgram::BaseProgram;

    static UniformValues uniformValues(const bool isText,
                                       const style::SymbolPropertyValues&,
                                       const SymbolSizeData& sizeData,
                                       const Size& texsize,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const RenderTile&,
                                       const TransformState&,
                                       const SymbolSDFPart);
};

using SymbolSDFIconProgram = SymbolSDFProgram<style::IconPaintProperties>;
using SymbolSDFTextProgram = SymbolSDFProgram<style::TextPaintProperties>;

using SymbolLayoutVertex = SymbolLayoutAttributes::Vertex;
using SymbolIconAttributes = SymbolIconProgram::Attributes;
using SymbolTextAttributes = SymbolSDFTextProgram::Attributes;

} // namespace mbgl
