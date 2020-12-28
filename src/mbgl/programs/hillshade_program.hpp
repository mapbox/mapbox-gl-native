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
MBGL_DEFINE_UNIFORM_SCALAR(Color, shadow);
MBGL_DEFINE_UNIFORM_SCALAR(Color, highlight);
MBGL_DEFINE_UNIFORM_SCALAR(Color, accent);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, light);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, latrange);
} // namespace uniforms

class HillshadeProgram : public Program<
    HillshadeProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<
        attributes::pos,
        attributes::texture_pos>,
    TypeList<
        uniforms::matrix,
        uniforms::highlight,
        uniforms::shadow,
        uniforms::accent,
        uniforms::light,
        uniforms::latrange>,
    TypeList<
        textures::image>,
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
