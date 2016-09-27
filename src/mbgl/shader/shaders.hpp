#pragma once

#include <mbgl/gl/gl.hpp>

#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/shader/circle_shader.hpp>

namespace mbgl {

class Shaders {
public:
    Shaders(gl::Context& context, Shader::Defines defines = Shader::None)
        : plain(context, defines),
          outline(context, defines),
          outlinePattern(context, defines),
          line(context, defines),
          linesdf(context, defines),
          linepattern(context, defines),
          pattern(context, defines),
          icon(context, defines),
          raster(context, defines),
          sdfGlyph(context, defines),
          sdfIcon(context, defines),
          collisionBox(context),
          circle(context, defines) {
    }

    PlainShader plain;
    OutlineShader outline;
    OutlinePatternShader outlinePattern;
    LineShader line;
    LineSDFShader linesdf;
    LinepatternShader linepattern;
    PatternShader pattern;
    IconShader icon;
    RasterShader raster;
    SDFShader sdfGlyph;
    SDFShader sdfIcon;
    CollisionBoxShader collisionBox;
    CircleShader circle;

    VertexArrayObject coveringPlainArray;
    VertexArrayObject coveringRasterArray;
    VertexArrayObject backgroundPatternArray;
    VertexArrayObject backgroundArray;
};

} // namespace mbgl
