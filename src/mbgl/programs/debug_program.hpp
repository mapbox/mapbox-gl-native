#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/debug.hpp>
#include <mbgl/style/properties.hpp>

namespace mbgl {

class DebugProgram : public Program<
    DebugProgram,
    shaders::debug,
    gfx::PrimitiveType::Line,
    TypeList<
        attributes::a_pos>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_color>,
    TypeList<>,
    style::Properties<>>
{
public:
    using Program::Program;
};

using DebugLayoutVertex = DebugProgram::LayoutVertex;
using DebugAttributes = DebugProgram::AttributeList;

} // namespace mbgl
