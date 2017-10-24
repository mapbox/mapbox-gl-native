#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/collision_box.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

using CollisionBoxAttributes = gl::Attributes<
    attributes::a_pos,
    attributes::a_anchor_pos,
    attributes::a_extrude>;

struct CollisionBoxOpacityAttributes : gl::Attributes<attributes::a_placed> {
    static Vertex vertex(bool placed, bool notUsed) {
        return Vertex {
            {{ static_cast<uint8_t>(placed), static_cast<uint8_t>(notUsed)  }}
        };
    }
};

class CollisionBoxProgram : public Program<
    shaders::collision_box,
    gl::Line,
    CollisionBoxAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_camera_to_center_distance>,
    style::Properties<>>
{
public:
    using Program::Program;

    static LayoutVertex vertex(Point<float> a, Point<float> anchor, Point<float> o) {
        return LayoutVertex {
            {{
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y)
            }},
            {{
                static_cast<int16_t>(anchor.x),
                static_cast<int16_t>(anchor.y)
            }},
            {{
                static_cast<int16_t>(::round(o.x)),
                static_cast<int16_t>(::round(o.y))
            }}
        };
    }
};

using CollisionBoxVertex = CollisionBoxProgram::LayoutVertex;

} // namespace mbgl
