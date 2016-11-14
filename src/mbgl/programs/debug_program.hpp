#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/debug.hpp>

namespace mbgl {

using DebugAttributes = gl::Attributes<
    attributes::a_pos>;

class DebugProgram : public Program<
    shaders::debug,
    gl::Line,
    DebugAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_color>>
{
public:
    using Program::Program;
};

using DebugVertex = DebugProgram::Vertex;

} // namespace mbgl
