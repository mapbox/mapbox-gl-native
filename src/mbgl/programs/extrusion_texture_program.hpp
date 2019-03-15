#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class ExtrusionTextureProgram : public Program<
    ExtrusionTextureProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<attributes::a_pos>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_world,
        uniforms::u_opacity>,
    TypeList<
        textures::u_image>,
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
using ExtrusionTextureAttributes = ExtrusionTextureProgram::AttributeList;

} // namespace mbgl
