#pragma once

#include <mbgl/programs/circle_program.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/debug_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/programs/program_parameters.hpp>

namespace mbgl {

class Programs {
public:
    Programs(gl::Context& context, const ProgramParameters& programParameters)
        : circle(context, programParameters),
          fill(context, programParameters),
          fillPattern(context, programParameters),
          fillOutline(context, programParameters),
          fillOutlinePattern(context, programParameters),
          line(context, programParameters),
          lineSDF(context, programParameters),
          linePattern(context, programParameters),
          raster(context, programParameters),
          symbolIcon(context, programParameters),
          symbolIconSDF(context, programParameters),
          symbolGlyph(context, programParameters),
          debug(context, ProgramParameters(programParameters.pixelRatio, false)),
          collisionBox(context, ProgramParameters(programParameters.pixelRatio, false)) {
    }

    CircleProgram circle;
    FillProgram fill;
    FillPatternProgram fillPattern;
    FillOutlineProgram fillOutline;
    FillOutlinePatternProgram fillOutlinePattern;
    LineProgram line;
    LineSDFProgram lineSDF;
    LinePatternProgram linePattern;
    RasterProgram raster;
    SymbolIconProgram symbolIcon;
    SymbolSDFProgram symbolIconSDF;
    SymbolSDFProgram symbolGlyph;

    DebugProgram debug;
    CollisionBoxProgram collisionBox;
};

} // namespace mbgl
