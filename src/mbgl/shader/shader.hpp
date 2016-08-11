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

    virtual void bind(GLbyte *offset) = 0;

    enum Defines : bool {
        None = false,
        Overdraw = true,
    };

protected:
    Shader(const char* name_,
           const char* vertex,
           const char* fragment,
           gl::ObjectStore&,
           Defines defines = Defines::None);

    static constexpr GLint         a_pos = 0;
    static constexpr GLint     a_extrude = 1;
    static constexpr GLint      a_offset = 2;
    static constexpr GLint        a_data = 3;
    static constexpr GLint a_texture_pos = 4;

private:
    bool compileShader(gl::UniqueShader&, const GLchar *source);

    gl::UniqueProgram program;
    gl::UniqueShader vertexShader;
    gl::UniqueShader fragmentShader;
};

} // namespace mbgl
