#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

class CollisionBoxShader : public Shader {
public:
    CollisionBoxShader(gl::ObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>              u_matrix      = {"u_matrix",      *this};
    Uniform<GLfloat>              u_scale       = {"u_scale",       *this};
    Uniform<GLfloat>              u_zoom        = {"u_zoom",        *this};
    Uniform<GLfloat>              u_maxzoom     = {"u_maxzoom",     *this};
};

} // namespace mbgl
