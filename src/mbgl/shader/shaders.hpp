#pragma once

#include <mbgl/shader/circle_shader.hpp>
#include <mbgl/shader/fill_shader.hpp>
#include <mbgl/shader/fill_pattern_shader.hpp>
#include <mbgl/shader/fill_outline_shader.hpp>
#include <mbgl/shader/fill_outline_pattern_shader.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/line_sdf_shader.hpp>
#include <mbgl/shader/line_pattern_shader.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/symbol_icon_shader.hpp>
#include <mbgl/shader/symbol_sdf_shader.hpp>

#include <mbgl/shader/collision_box_shader.hpp>

namespace mbgl {

class Shaders {
public:
    Shaders(gl::Context& context, gl::Shader::Defines defines = gl::Shader::None)
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
          collisionBox(context) {
    }

    CircleShader circle;
    FillShader fill;
    FillPatternShader fillPattern;
    FillOutlineShader fillOutline;
    FillOutlinePatternShader fillOutlinePattern;
    LineShader line;
    LineSDFShader lineSDF;
    LinePatternShader linePattern;
    RasterShader raster;
    SymbolIconShader symbolIcon;
    SymbolSDFShader symbolIconSDF;
    SymbolSDFShader symbolGlyph;

    CollisionBoxShader collisionBox;

    gl::VertexArrayObject coveringPlainArray;
    gl::VertexArrayObject coveringRasterArray;
    gl::VertexArrayObject backgroundPatternArray;
    gl::VertexArrayObject backgroundArray;
};

} // namespace mbgl
