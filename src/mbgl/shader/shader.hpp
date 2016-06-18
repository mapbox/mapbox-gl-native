#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class Shader : private util::noncopyable {
public:
    ~Shader();
    const char* name;

    GLuint getID() const {
        return program.get();
    }

    GLuint getOverdrawID() const {
        return programOverdraw ? *programOverdraw : 0;
    }

    virtual void bind(GLbyte *offset) = 0;

protected:
    Shader(const char* name_, const char* vertex, const char* fragment, gl::ObjectStore&);
    GLint a_pos = -1;

private:
    bool compileShader(gl::UniqueShader&, const GLchar *source);
    void linkProgram(gl::UniqueProgram&);

    gl::UniqueProgram program;
    gl::UniqueShader vertexShader;
    gl::UniqueShader fragmentShader;

    mbgl::optional<gl::UniqueProgram> programOverdraw;
    mbgl::optional<gl::UniqueShader> overdrawShader;
};

} // namespace mbgl
