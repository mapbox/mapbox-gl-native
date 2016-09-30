#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace gl {

class Context;

class Shader : private util::noncopyable {
public:
    ~Shader();
    const char* name;

    ProgramID getID() const {
        return program.get();
    }

    UniformLocation getUniformLocation(const char* uniform) const;

    enum Defines : bool {
        None = false,
        Overdraw = true,
    };

protected:
    Shader(const char* name_,
           const char* vertex,
           const char* fragment,
           Context&,
           Defines defines = Defines::None);

public:
    static constexpr AttributeLocation         a_pos = 0;
    static constexpr AttributeLocation     a_extrude = 1;
    static constexpr AttributeLocation      a_offset = 2;
    static constexpr AttributeLocation        a_data = 3;
    static constexpr AttributeLocation a_texture_pos = 4;

private:
    bool compileShader(UniqueShader&, const char *source);

    UniqueProgram program;
    UniqueShader vertexShader;
    UniqueShader fragmentShader;
};

} // namespace gl
} // namespace mbgl
