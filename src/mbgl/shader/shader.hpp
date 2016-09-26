#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class Shader : private util::noncopyable {
public:
    ~Shader();
    const char* name;

    gl::ProgramID getID() const {
        return program.get();
    }

    gl::UniformLocation getUniformLocation(const char* uniform) const;

    enum Defines : bool {
        None = false,
        Overdraw = true,
    };

protected:
    Shader(const char* name_,
           const char* vertex,
           const char* fragment,
           gl::Context&,
           Defines defines = Defines::None);

public:
    static constexpr gl::AttributeLocation         a_pos = 0;
    static constexpr gl::AttributeLocation     a_extrude = 1;
    static constexpr gl::AttributeLocation      a_offset = 2;
    static constexpr gl::AttributeLocation        a_data = 3;
    static constexpr gl::AttributeLocation a_texture_pos = 4;

private:
    bool compileShader(gl::UniqueShader&, const char *source);

    gl::UniqueProgram program;
    gl::UniqueShader vertexShader;
    gl::UniqueShader fragmentShader;
};

} // namespace mbgl
