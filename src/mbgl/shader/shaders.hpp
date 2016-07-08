#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>

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
    Shaders(gl::ObjectStore& store, Shader::Defines defines = Shader::None)
        : plain(store, defines),
          outline(store, defines),
          outlinePattern(store, defines),
          line(store, defines),
          linesdf(store, defines),
          linepattern(store, defines),
          pattern(store, defines),
          icon(store, defines),
          raster(store, defines),
          sdfGlyph(store, defines),
          sdfIcon(store, defines),
          collisionBox(store),
          circle(store, defines)
          {}

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
