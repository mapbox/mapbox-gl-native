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
          symbolSDFIconFill(context, programParameters),
          symbolSDFIconHalo(context, programParameters),
          symbolSDFTextFill(context, programParameters),
          symbolSDFTextHalo(context, programParameters),
          debug(context, ProgramParameters(programParameters.pixelRatio, false, programParameters.cacheDir)),
          collisionBox(context, ProgramParameters(programParameters.pixelRatio, false, programParameters.cacheDir)) {
    }

    CircleProgram circle;
    ExtrusionTextureProgram extrusionTexture;
    FillProgram fill;
    FillExtrusionProgram fillExtrusion;
    FillExtrusionPatternProgram fillExtrusionPattern;
    FillPatternProgram fillPattern;
    FillOutlineProgram fillOutline;
    FillOutlinePatternProgram fillOutlinePattern;
    LineProgram line;
    LineSDFProgram lineSDF;
    LinePatternProgram linePattern;
    RasterProgram raster;
    SymbolIconProgram symbolIcon;
    SymbolSDFIconFillProgram symbolSDFIconFill;
    SymbolSDFIconHaloProgram symbolSDFIconHalo;
    SymbolSDFTextFillProgram symbolSDFTextFill;
    SymbolSDFTextHaloProgram symbolSDFTextHalo;

    DebugProgram debug;
    CollisionBoxProgram collisionBox;
};

} // namespace mbgl
