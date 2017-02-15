#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/extrusion_texture.hpp>
//#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
//#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

#include <string>

namespace mbgl {

namespace uniforms {
    MBGL_DEFINE_UNIFORM_SCALAR(uint16_t, u_xdim);
    MBGL_DEFINE_UNIFORM_SCALAR(uint16_t, u_ydim);
} // namespace uniforms

struct ExtrusionTextureLayoutAttributes : gl::Attributes<
attributes::a_pos>
{};

struct ExtrusionTextureUniforms : gl::Uniforms<
uniforms::u_matrix,
uniforms::u_xdim,
uniforms::u_ydim,
uniforms::u_texture,
uniforms::u_opacity>
{};

class ExtrusionTextureProgram : public Program<
shaders::extrusion_texture,
gl::Triangle,
ExtrusionTextureLayoutAttributes,
ExtrusionTextureUniforms,
style::FillExtrusionPaintProperties>
{
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p) {
        return LayoutVertex {
            {{
                p.x,
                p.y
            }}
        };
    }
};

using ExtrusionTextureLayoutVertex = ExtrusionTextureProgram::LayoutVertex;
using ExtrusionTextureAttributes = ExtrusionTextureProgram::Attributes;

} // namespace mbgl
