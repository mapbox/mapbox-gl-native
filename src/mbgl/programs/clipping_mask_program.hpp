#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/clipping_mask.hpp>
#include <mbgl/style/properties.hpp>

namespace mbgl {

class ClippingMaskProgram : public Program<
    shaders::clipping_mask,
    gl::Triangle,
    PositionOnlyLayoutAttributes,
    gl::Uniforms<
        uniforms::u_matrix>,
    style::Properties<>>
{
public:
    using Program::Program;
};

using ClippingMaskLayoutVertex = ClippingMaskProgram::LayoutVertex;
using ClippingMaskAttributes = ClippingMaskProgram::Attributes;

} // namespace mbgl
