#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/convert.hpp>

#include <memory>

namespace mbgl {
namespace gl {

UniformLocation uniformLocation(ProgramID id, const char* name) {
    return MBGL_CHECK_ERROR(glGetUniformLocation(id, name));
}

template <>
void bindUniform<float>(UniformLocation location, const float& t) {
    MBGL_CHECK_ERROR(glUniform1f(location, t));
}

template <>
void bindUniform<int32_t>(UniformLocation location, const int32_t& t) {
    MBGL_CHECK_ERROR(glUniform1i(location, t));
}

template <>
void bindUniform<std::array<float, 2>>(UniformLocation location, const std::array<float, 2>& t) {
    MBGL_CHECK_ERROR(glUniform2fv(location, 1, t.data()));
}

template <>
void bindUniform<std::array<float, 3>>(UniformLocation location, const std::array<float, 3>& t) {
    MBGL_CHECK_ERROR(glUniform3fv(location, 1, t.data()));
}

template <>
void bindUniform<std::array<float, 4>>(UniformLocation location, const std::array<float, 4>& t) {
    MBGL_CHECK_ERROR(glUniform4fv(location, 1, t.data()));
}

template <>
void bindUniform<std::array<double, 4>>(UniformLocation location, const std::array<double, 4>& t) {
    MBGL_CHECK_ERROR(glUniformMatrix2fv(location, 1, GL_FALSE, util::convert<float>(t).data()));
}

template <>
void bindUniform<std::array<double, 9>>(UniformLocation location, const std::array<double, 9>& t) {
    MBGL_CHECK_ERROR(glUniformMatrix3fv(location, 1, GL_FALSE, util::convert<float>(t).data()));
}

template <>
void bindUniform<std::array<double, 16>>(UniformLocation location, const std::array<double, 16>& t) {
    MBGL_CHECK_ERROR(glUniformMatrix4fv(location, 1, GL_FALSE, util::convert<float>(t).data()));
}


template <>
void bindUniform<bool>(UniformLocation location, const bool& t) {
    return bindUniform(location, int32_t(t));
}

template <>
void bindUniform<uint8_t>(UniformLocation location, const uint8_t& t) {
    bindUniform(location, int32_t(t));
}

template <>
void bindUniform<Color>(UniformLocation location, const Color& t) {
    bindUniform(location, std::array<float, 4> {{ t.r, t.g, t.b, t.a }});
}

template <>
void bindUniform<Size>(UniformLocation location, const Size& t) {
    bindUniform(location, util::convert<float>(std::array<uint32_t, 2> {{ t.width, t.height }}));
}

template <>
void bindUniform<std::array<uint16_t, 2>>(UniformLocation location, const std::array<uint16_t, 2>& t) {
    bindUniform(location, util::convert<float>(t));
}

// Add more as needed.

#ifndef NDEBUG

ActiveUniforms activeUniforms(ProgramID id) {
    ActiveUniforms active;

    GLint count;
    GLint maxLength;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count));
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));

    auto name = std::make_unique<GLchar[]>(maxLength);
    GLsizei length;
    GLint size;
    GLenum type;
    for (GLint index = 0; index < count; index++) {
        MBGL_CHECK_ERROR(
            glGetActiveUniform(id, index, maxLength, &length, &size, &type, name.get()));
        active.emplace(
            std::string{ name.get(), static_cast<size_t>(length) },
            ActiveUniform{ static_cast<size_t>(size), static_cast<UniformDataType>(type) });
    }

    return active;
}

template <>
bool verifyUniform<float>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 && uniform.type == UniformDataType::Float);
    return true;
}

template <>
bool verifyUniform<std::array<float, 2>>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 && uniform.type == UniformDataType::FloatVec2);
    return true;
}

template <>
bool verifyUniform<std::array<float, 3>>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 && uniform.type == UniformDataType::FloatVec3);
    return true;
}

template <>
bool verifyUniform<std::array<double, 16>>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 && uniform.type == UniformDataType::FloatMat4);
    return true;
}

template <>
bool verifyUniform<bool>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 &&
           (uniform.type == UniformDataType::Bool ||
            uniform.type == UniformDataType::Int ||
            uniform.type == UniformDataType::Float));
    return true;
}

template <>
bool verifyUniform<uint8_t>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 &&
           (uniform.type == UniformDataType::Int ||
            uniform.type == UniformDataType::Float ||
            uniform.type == UniformDataType::Sampler2D));
    return true;
}

template <>
bool verifyUniform<Color>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 && uniform.type == UniformDataType::FloatVec4);
    return true;
}

template <>
bool verifyUniform<Size>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 && uniform.type == UniformDataType::FloatVec2);
    return true;
}

template <>
bool verifyUniform<std::array<uint16_t, 2>>(const ActiveUniform& uniform) {
    assert(uniform.size == 1 &&
           (uniform.type == UniformDataType::IntVec2 ||
            uniform.type == UniformDataType::FloatVec2));
    return true;
}

// Add more as needed.

#endif

} // namespace gl
} // namespace mbgl
