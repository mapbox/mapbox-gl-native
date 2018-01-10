#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/hillshade.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/hillshade_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_shadow);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_highlight);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_accent);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_light);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_latrange);
} // namespace uniforms

class HillshadeProgram : public Program<
    shaders::hillshade,
    gl::Triangle,
    gl::Attributes<
        attributes::a_pos,
        attributes::a_texture_pos>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_image,
        uniforms::u_highlight,
        uniforms::u_shadow,
        uniforms::u_accent,
        uniforms::u_light,
        uniforms::u_latrange>,
    style::HillshadePaintProperties>{
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

using HillshadeLayoutVertex = HillshadeProgram::LayoutVertex;
using HillshadeAttributes = HillshadeProgram::Attributes;

} // namespace mbgl
