#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/hillshade_prepare.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, u_dimension);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_maxzoom);
} // namespace uniforms

class HillshadePrepareProgram : public Program<
    shaders::hillshade_prepare,
    gl::Triangle,
    gl::Attributes<
        attributes::a_pos,
        attributes::a_texture_pos>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_dimension,
        uniforms::u_zoom,
        uniforms::u_maxzoom,
        uniforms::u_image>,
    style::Properties<>> {
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p, Point<uint16_t> t) {
        return LayoutVertex {
            {{
                p.x,
                p.y
            }},
            {{
                t.x,
                t.y
            }}
        };
    }
};

using HillshadePrepareLayoutVertex = HillshadePrepareProgram::LayoutVertex;
using HillshadePrepareAttributes = HillshadePrepareProgram::Attributes;

} // namespace mbgl
