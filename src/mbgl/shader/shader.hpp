#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class Shader : private util::noncopyable {
public:
    Shader(const GLchar *name, const GLchar *vertex, const GLchar *fragment, gl::ObjectStore&);

    ~Shader();
    const GLchar *name;

    GLuint getID() const {
        return program.get();
    }

    virtual void bind(GLbyte *offset) = 0;

protected:
    GLint a_pos = -1;

private:
    bool compileShader(gl::UniqueShader&, const GLchar *source[]);

    gl::UniqueProgram program;
    gl::UniqueShader vertexShader;
    gl::UniqueShader fragmentShader;
};

} // namespace mbgl
