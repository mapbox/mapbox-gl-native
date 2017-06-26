#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    if (location != -1) MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
}

AttributeLocation locationToBindAttribute(std::set<std::string>& set, std::string attributeName) {
    auto it = set.find(attributeName);
    if (it != set.end()) {
        return std::distance(set.begin(), it);
    } else {
        return AttributeLocation(-1);
    }
}

int32_t getActiveAttributeCount(ProgramID id) {
    GLint numAttributes;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &numAttributes));
    return numAttributes;
}

int32_t getMaxAttributeNameLength(ProgramID id) {
    GLint nameLength;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLength));
    return nameLength;
}

std::string getAttributeName(ProgramID id, int32_t maxLength, AttributeLocation location) {
    std::string attributeName;
    attributeName.resize(maxLength);
    MBGL_CHECK_ERROR(glGetActiveAttrib(id, static_cast<GLuint>(location),
                                       static_cast<GLsizei>(maxLength), nullptr, nullptr, nullptr,
                                       const_cast<char*>(attributeName.data())));
    attributeName.resize(strlen(attributeName.c_str()));
    return attributeName;
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
