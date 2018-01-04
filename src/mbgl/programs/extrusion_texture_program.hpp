#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/extrusion_texture.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class ExtrusionTextureProgram : public Program<
    shaders::extrusion_texture,
    gl::Triangle,
    gl::Attributes<attributes::a_pos>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_world,
        uniforms::u_image,
        uniforms::u_opacity>,
    style::Properties<>> {
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p) {
        return LayoutVertex{
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
