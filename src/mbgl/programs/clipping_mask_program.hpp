#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/style/properties.hpp>

namespace mbgl {

class ClippingMaskProgram : public Program<
    ClippingMaskProgram,
    gfx::PrimitiveType::Triangle,
    PositionOnlyLayoutAttributes,
    TypeList<
        uniforms::matrix>,
    TypeList<>,
    style::Properties<>>
{
public:
    using Program::Program;
};

using ClippingMaskLayoutVertex = ClippingMaskProgram::LayoutVertex;
using ClippingMaskAttributes = ClippingMaskProgram::AttributeList;

} // namespace mbgl
