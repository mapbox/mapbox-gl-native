#pragma once

#include <mbgl/programs/circle_program.hpp>
#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
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
          extrusionTexture(context, programParameters),
          fill(context, programParameters),
          fillExtrusion(context, programParameters),
          fillExtrusionPattern(context, programParameters),
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
          debug(context, programParameters),
          collisionBox(context, programParameters) {
    }

    ProgramMap<CircleProgram> circle;
    ExtrusionTextureProgram extrusionTexture;
    ProgramMap<FillProgram> fill;
    ProgramMap<FillExtrusionProgram> fillExtrusion;
    ProgramMap<FillExtrusionPatternProgram> fillExtrusionPattern;
    ProgramMap<FillPatternProgram> fillPattern;
    ProgramMap<FillOutlineProgram> fillOutline;
    ProgramMap<FillOutlinePatternProgram> fillOutlinePattern;
    ProgramMap<LineProgram> line;
    ProgramMap<LineSDFProgram> lineSDF;
    ProgramMap<LinePatternProgram> linePattern;
    RasterProgram raster;
    ProgramMap<SymbolIconProgram> symbolIcon;
    ProgramMap<SymbolSDFIconProgram> symbolIconSDF;
    ProgramMap<SymbolSDFTextProgram> symbolGlyph;

    DebugProgram debug;
    CollisionBoxProgram collisionBox;
};

} // namespace mbgl
