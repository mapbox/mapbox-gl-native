#pragma once

#include <mbgl/programs/circle_program.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>

namespace mbgl {

class Programs {
public:
    Programs(gl::Context& context, ProgramDefines defines = ProgramDefines::None)
        : circle(context, defines),
          fill(context, defines),
          fillPattern(context, defines),
          fillOutline(context, defines),
          fillOutlinePattern(context, defines),
          line(context, defines),
          lineSDF(context, defines),
          linePattern(context, defines),
          raster(context, defines),
          symbolIcon(context, defines),
          symbolIconSDF(context, defines),
          symbolGlyph(context, defines),
          collisionBox(context, ProgramDefines::None) {
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

    CollisionBoxProgram collisionBox;
};

} // namespace mbgl
