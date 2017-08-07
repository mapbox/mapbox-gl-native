#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/debug.hpp>
#include <mbgl/style/properties.hpp>

namespace mbgl {

class DebugProgram : public Program<
    shaders::debug,
    gl::Line,
    gl::Attributes<
        attributes::a_pos>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_color>,
    style::Properties<>>
{
public:
    using Program::Program;
};

using DebugLayoutVertex = DebugProgram::LayoutVertex;
using DebugAttributes = DebugProgram::Attributes;

} // namespace mbgl
