#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/extrusion_texture.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

#include <string>

namespace mbgl {

struct ExtrusionTextureLayoutAttributes : gl::Attributes<
attributes::a_pos>
{};

struct ExtrusionTextureUniforms : gl::Uniforms<
uniforms::u_matrix,
uniforms::u_world,
uniforms::u_image,
uniforms::u_opacity>
{};

class ExtrusionTextureProgram : public Program<
shaders::extrusion_texture,
gl::Triangle,
ExtrusionTextureLayoutAttributes,
ExtrusionTextureUniforms,
style::PaintProperties<>>
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
