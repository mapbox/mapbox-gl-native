#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/collision_box.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_scale);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_maxzoom);
} // namespace uniforms

class CollisionBoxProgram : public Program<
    shaders::collision_box,
    gl::Attributes<
        attributes::a_pos,
        attributes::a_extrude,
        attributes::a_data<2>>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_scale,
        uniforms::u_zoom,
        uniforms::u_maxzoom>>
{
public:
    using Program::Program;

    static Vertex vertex(Point<float> a, Point<float> o, float maxzoom, float placementZoom) {
        return Vertex {
            {
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y)
            },
            {
                static_cast<int16_t>(::round(o.x)),
                static_cast<int16_t>(::round(o.y))
            },
            {
                static_cast<uint8_t>(maxzoom * 10),
                static_cast<uint8_t>(placementZoom * 10)
            }
        };
    }
};

using CollisionBoxVertex = CollisionBoxProgram::Vertex;

} // namespace mbgl
