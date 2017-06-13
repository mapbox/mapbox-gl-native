#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
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
