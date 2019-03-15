#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/hillshade_prepare_program.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
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
    HillshadeProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<
        attributes::a_pos,
        attributes::a_texture_pos>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_highlight,
        uniforms::u_shadow,
        uniforms::u_accent,
        uniforms::u_light,
        uniforms::u_latrange>,
    TypeList<
        textures::u_image>,
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
using HillshadeAttributes = HillshadeProgram::AttributeList;

class HillshadeLayerPrograms final : public LayerTypePrograms  {
public:
    HillshadeLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : hillshade(context, programParameters),
          hillshadePrepare(context, programParameters) {}
    HillshadeProgram hillshade;
    HillshadePrepareProgram hillshadePrepare;
};

} // namespace mbgl
