#pragma once

#include <mbgl/programs/background_program.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/programs/clipping_mask_program.hpp>
#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/programs/heatmap_texture_program.hpp>
#include <mbgl/programs/hillshade_program.hpp>
#include <mbgl/programs/hillshade_prepare_program.hpp>
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
        : background(context, programParameters),
          backgroundPattern(context, programParameters),
          circle(context, programParameters),
          extrusionTexture(context, programParameters),
          fill(context, programParameters),
          fillExtrusion(context, programParameters),
          fillExtrusionPattern(context, programParameters),
          fillPattern(context, programParameters),
          fillOutline(context, programParameters),
          fillOutlinePattern(context, programParameters),
          heatmap(context, programParameters),
          heatmapTexture(context, programParameters),
          hillshade(context, programParameters),
          hillshadePrepare(context, programParameters),
          line(context, programParameters),
          lineSDF(context, programParameters),
          linePattern(context, programParameters),
          raster(context, programParameters),
          symbolIcon(context, programParameters),
          symbolIconSDF(context, programParameters),
          symbolGlyph(context, programParameters),
          debug(context, programParameters),
          collisionBox(context, programParameters),
          collisionCircle(context, programParameters),
          clippingMask(context, programParameters) {
    }

    BackgroundProgram background;
    BackgroundPatternProgram backgroundPattern;
    ProgramMap<CircleProgram> circle;
    ExtrusionTextureProgram extrusionTexture;
    ProgramMap<FillProgram> fill;
    ProgramMap<FillExtrusionProgram> fillExtrusion;
    ProgramMap<FillExtrusionPatternProgram> fillExtrusionPattern;
    ProgramMap<FillPatternProgram> fillPattern;
    ProgramMap<FillOutlineProgram> fillOutline;
    ProgramMap<FillOutlinePatternProgram> fillOutlinePattern;
    ProgramMap<HeatmapProgram> heatmap;
    HeatmapTextureProgram heatmapTexture;
    HillshadeProgram hillshade;
    HillshadePrepareProgram hillshadePrepare;
    ProgramMap<LineProgram> line;
    ProgramMap<LineSDFProgram> lineSDF;
    ProgramMap<LinePatternProgram> linePattern;
    RasterProgram raster;
    ProgramMap<SymbolIconProgram> symbolIcon;
    ProgramMap<SymbolSDFIconProgram> symbolIconSDF;
    ProgramMap<SymbolSDFTextProgram> symbolGlyph;

    DebugProgram debug;
    CollisionBoxProgram collisionBox;
    CollisionCircleProgram collisionCircle;
    ClippingMaskProgram clippingMask;
};

} // namespace mbgl
