#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    assert(location < 8);
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
}

std::set<std::string> getActiveAttributes(ProgramID id) {
    std::set<std::string> activeAttributes;

    GLint attributeCount;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &attributeCount));

    GLint maxAttributeLength;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLength));

    std::string attributeName;
    attributeName.resize(maxAttributeLength);

    GLsizei actualLength;
    GLint size;
    GLenum type;

    for (int32_t i = 0; i < attributeCount; i++) {
        MBGL_CHECK_ERROR(glGetActiveAttrib(id, i, maxAttributeLength, &actualLength, &size, &type, &attributeName[0]));
        activeAttributes.emplace(std::string(attributeName, 0, actualLength));
    }

    return activeAttributes;
}

void DisabledAttribute::bind(Context&, AttributeLocation location, std::size_t) const {
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
}

template <class T> DataType DataTypeOf    = static_cast<DataType>(0);
template <> DataType DataTypeOf< int8_t>  = DataType::Byte;
template <> DataType DataTypeOf<uint8_t>  = DataType::UnsignedByte;
template <> DataType DataTypeOf< int16_t> = DataType::Short;
template <> DataType DataTypeOf<uint16_t> = DataType::UnsignedShort;
template <> DataType DataTypeOf< int32_t> = DataType::Integer;
template <> DataType DataTypeOf<uint32_t> = DataType::UnsignedInteger;
template <> DataType DataTypeOf<float>    = DataType::Float;

template <class T, std::size_t N>
void AttributeBinding<T, N>::bind(Context& context, AttributeLocation location, std::size_t vertexOffset) const {
    // FillProgram will attempt to bind at location -1 because it includes
    // a fill-outline-color paint property but does not use it or have an
    // a_outline_color shader attribute - in this case, we have nothing to bind.
    if (location == -1) return;

    context.vertexBuffer = vertexBuffer;
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttribPointer(
        location,
        static_cast<GLint>(attributeSize),
        static_cast<GLenum>(DataTypeOf<T>),
        static_cast<GLboolean>(false),
        static_cast<GLsizei>(vertexSize),
        reinterpret_cast<GLvoid*>(attributeOffset + (vertexSize * vertexOffset))));
}

template class AttributeBinding<uint8_t, 1>;
template class AttributeBinding<uint8_t, 2>;
template class AttributeBinding<uint8_t, 3>;
template class AttributeBinding<uint8_t, 4>;

template class AttributeBinding<uint16_t, 1>;
template class AttributeBinding<uint16_t, 2>;
template class AttributeBinding<uint16_t, 3>;
template class AttributeBinding<uint16_t, 4>;

template class AttributeBinding<int16_t, 1>;
template class AttributeBinding<int16_t, 2>;
template class AttributeBinding<int16_t, 3>;
template class AttributeBinding<int16_t, 4>;

template class AttributeBinding<float, 1>;
template class AttributeBinding<float, 2>;
template class AttributeBinding<float, 3>;
template class AttributeBinding<float, 4>;

} // namespace gl
} // namespace mbgl
