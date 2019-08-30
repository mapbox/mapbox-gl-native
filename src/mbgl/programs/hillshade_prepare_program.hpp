#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, dimension);
MBGL_DEFINE_UNIFORM_SCALAR(float, maxzoom);
MBGL_DEFINE_UNIFORM_VECTOR(float, 4, unpack);
} // namespace uniforms

class HillshadePrepareProgram : public Program<
    HillshadePrepareProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<
        attributes::pos,
        attributes::texture_pos>,
    TypeList<
        uniforms::matrix,
        uniforms::dimension,
        uniforms::zoom,
        uniforms::maxzoom,
        uniforms::unpack>,
    TypeList<
        textures::image>,
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
using HillshadePrepareAttributes = HillshadePrepareProgram::AttributeList;

} // namespace mbgl
