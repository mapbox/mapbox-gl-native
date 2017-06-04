#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
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
void VariableAttributeBinding<T, N>::bind(Context& context,
                                          AttributeLocation location,
                                          optional<VariableAttributeBinding<T, N>>& oldBinding,
                                          std::size_t vertexOffset) const {
    if (oldBinding == *this) {
        return;
    }
    context.vertexBuffer = vertexBuffer;
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(location));
    oldBinding = *this;
    MBGL_CHECK_ERROR(glVertexAttribPointer(
        location,
        static_cast<GLint>(attributeSize),
        static_cast<GLenum>(DataTypeOf<T>),
        static_cast<GLboolean>(false),
        static_cast<GLsizei>(vertexSize),
        reinterpret_cast<GLvoid*>(attributeOffset + (vertexSize * vertexOffset))));
}

template class VariableAttributeBinding<uint8_t, 1>;
template class VariableAttributeBinding<uint8_t, 2>;
template class VariableAttributeBinding<uint8_t, 3>;
template class VariableAttributeBinding<uint8_t, 4>;

template class VariableAttributeBinding<uint16_t, 1>;
template class VariableAttributeBinding<uint16_t, 2>;
template class VariableAttributeBinding<uint16_t, 3>;
template class VariableAttributeBinding<uint16_t, 4>;

template class VariableAttributeBinding<int16_t, 1>;
template class VariableAttributeBinding<int16_t, 2>;
template class VariableAttributeBinding<int16_t, 3>;
template class VariableAttributeBinding<int16_t, 4>;

template class VariableAttributeBinding<float, 1>;
template class VariableAttributeBinding<float, 2>;
template class VariableAttributeBinding<float, 3>;
template class VariableAttributeBinding<float, 4>;

template <>
void ConstantAttributeBinding<uint8_t, 1>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint8_t, 1>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeBinding<uint8_t, 2>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint8_t, 2>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeBinding<uint8_t, 3>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint8_t, 3>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeBinding<uint8_t, 4>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint8_t, 4>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}


template <>
void ConstantAttributeBinding<uint16_t, 1>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint16_t, 1>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeBinding<uint16_t, 2>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint16_t, 2>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeBinding<uint16_t, 3>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint16_t, 3>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeBinding<uint16_t, 4>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<uint16_t, 4>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}


template <>
void ConstantAttributeBinding<int16_t, 1>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<int16_t, 1>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeBinding<int16_t, 2>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<int16_t, 2>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeBinding<int16_t, 3>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<int16_t, 3>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeBinding<int16_t, 4>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<int16_t, 4>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}


template <>
void ConstantAttributeBinding<float, 1>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<float, 1>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeBinding<float, 2>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<float, 2>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeBinding<float, 3>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<float, 3>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeBinding<float, 4>::bind(Context&, AttributeLocation location, optional<VariableAttributeBinding<float, 4>>& oldBinding, std::size_t) const {
    assert(location != 0);
    oldBinding = {};
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}

} // namespace gl
} // namespace mbgl
