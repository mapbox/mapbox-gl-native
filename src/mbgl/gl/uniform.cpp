#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/convert.hpp>

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

} // namespace gl
} // namespace mbgl
