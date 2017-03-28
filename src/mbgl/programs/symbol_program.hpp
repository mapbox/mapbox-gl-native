#pragma once

#include <mbgl/programs/program.hpp>
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
MBGL_DEFINE_UNIFORM_SCALAR(float, u_font_scale);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gamma_scale);
} // namespace uniforms

struct SymbolLayoutAttributes : gl::Attributes<
    attributes::a_pos_offset,
    attributes::a_data<4, uint16_t>,
    attributes::a_size>
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
                static_cast<uint16_t>(ty / 4)
            }},
            {{
                static_cast<uint8_t>(labelminzoom * 10), // 1/10 zoom levels: z16 == 160
                static_cast<uint8_t>(labelangle),
                static_cast<uint8_t>(minzoom * 10),
                static_cast<uint8_t>(::fmin(maxzoom, 25) * 10)
            }}
        };
    }
};

class SymbolIconProgram : public Program<
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
        uniforms::u_fadetexture>,
    style::IconPaintProperties>
{
public:
    using Program::Program;

    static UniformValues uniformValues(const style::SymbolPropertyValues&,
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
class SymbolSDFProgram : public Program<
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
        uniforms::u_font_scale,
        uniforms::u_gamma_scale,
        uniforms::u_pitch,
        uniforms::u_bearing,
        uniforms::u_aspect_ratio,
        uniforms::u_pitch_with_map,
        uniforms::u_is_halo>,
    PaintProperties>
{
public:
    using BaseProgram = Program<shaders::symbol_sdf,
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
            uniforms::u_font_scale,
            uniforms::u_gamma_scale,
            uniforms::u_pitch,
            uniforms::u_bearing,
            uniforms::u_aspect_ratio,
            uniforms::u_pitch_with_map,
            uniforms::u_is_halo>,
        PaintProperties>;
    
    using UniformValues = typename BaseProgram::UniformValues;
    

    
    using BaseProgram::BaseProgram;

    static UniformValues uniformValues(const style::SymbolPropertyValues&,
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
