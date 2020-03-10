#pragma once

#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/program.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/style/properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, overlay_scale);
}
class DebugProgram : public Program<DebugProgram,
                                    gfx::PrimitiveType::Line,
                                    TypeList<attributes::pos>,
                                    TypeList<uniforms::matrix, uniforms::color, uniforms::overlay_scale>,
                                    TypeList<textures::overlay>,
                                    style::Properties<>> {
public:
    using Program::Program;
};

using DebugLayoutVertex = DebugProgram::LayoutVertex;
using DebugAttributes = DebugProgram::AttributeList;

} // namespace mbgl
