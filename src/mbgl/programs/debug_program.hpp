#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/debug.hpp>

namespace mbgl {

class DebugProgram : public Program<
    shaders::debug,
    gl::Line,
    gl::Attributes<
        attributes::a_pos>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_color>>
{
public:
    using Program::Program;
};

} // namespace mbgl
