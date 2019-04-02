#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/util/type_list.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/optional.hpp>

#include <array>
#include <type_traits>
#include <cstddef>

#define MBGL_DEFINE_ATTRIBUTE(type_, n_, name_)                                                    \
    struct name_ {                                                                                 \
        using Type = ::mbgl::gfx::AttributeType<type_, n_>;                                        \
        static constexpr auto name() {                                                             \
            return #name_;                                                                         \
        }                                                                                          \
    }

#define MBGL_VERTEX_ALIGN __attribute__((aligned(4)))

namespace mbgl {
namespace gfx {

namespace {

template <typename, std::size_t> struct AttributeDataTypeOf;
template <> struct AttributeDataTypeOf<int8_t, 1> : std::integral_constant<AttributeDataType, AttributeDataType::Byte> {};
template <> struct AttributeDataTypeOf<int8_t, 2> : std::integral_constant<AttributeDataType, AttributeDataType::Byte2> {};
template <> struct AttributeDataTypeOf<int8_t, 3> : std::integral_constant<AttributeDataType, AttributeDataType::Byte3> {};
template <> struct AttributeDataTypeOf<int8_t, 4> : std::integral_constant<AttributeDataType, AttributeDataType::Byte4> {};
template <> struct AttributeDataTypeOf<uint8_t, 1> : std::integral_constant<AttributeDataType, AttributeDataType::UByte> {};
template <> struct AttributeDataTypeOf<uint8_t, 2> : std::integral_constant<AttributeDataType, AttributeDataType::UByte2> {};
template <> struct AttributeDataTypeOf<uint8_t, 3> : std::integral_constant<AttributeDataType, AttributeDataType::UByte3> {};
template <> struct AttributeDataTypeOf<uint8_t, 4> : std::integral_constant<AttributeDataType, AttributeDataType::UByte4> {};
template <> struct AttributeDataTypeOf<int16_t, 1> : std::integral_constant<AttributeDataType, AttributeDataType::Short> {};
template <> struct AttributeDataTypeOf<int16_t, 2> : std::integral_constant<AttributeDataType, AttributeDataType::Short2> {};
template <> struct AttributeDataTypeOf<int16_t, 3> : std::integral_constant<AttributeDataType, AttributeDataType::Short3> {};
template <> struct AttributeDataTypeOf<int16_t, 4> : std::integral_constant<AttributeDataType, AttributeDataType::Short4> {};
template <> struct AttributeDataTypeOf<uint16_t, 1> : std::integral_constant<AttributeDataType, AttributeDataType::UShort> {};
template <> struct AttributeDataTypeOf<uint16_t, 2> : std::integral_constant<AttributeDataType, AttributeDataType::UShort2> {};
template <> struct AttributeDataTypeOf<uint16_t, 3> : std::integral_constant<AttributeDataType, AttributeDataType::UShort3> {};
template <> struct AttributeDataTypeOf<uint16_t, 4> : std::integral_constant<AttributeDataType, AttributeDataType::UShort4> {};
template <> struct AttributeDataTypeOf<int32_t, 1> : std::integral_constant<AttributeDataType, AttributeDataType::Int> {};
template <> struct AttributeDataTypeOf<int32_t, 2> : std::integral_constant<AttributeDataType, AttributeDataType::Int2> {};
template <> struct AttributeDataTypeOf<int32_t, 3> : std::integral_constant<AttributeDataType, AttributeDataType::Int3> {};
template <> struct AttributeDataTypeOf<int32_t, 4> : std::integral_constant<AttributeDataType, AttributeDataType::Int4> {};
template <> struct AttributeDataTypeOf<uint32_t, 1> : std::integral_constant<AttributeDataType, AttributeDataType::UInt> {};
template <> struct AttributeDataTypeOf<uint32_t, 2> : std::integral_constant<AttributeDataType, AttributeDataType::UInt2> {};
template <> struct AttributeDataTypeOf<uint32_t, 3> : std::integral_constant<AttributeDataType, AttributeDataType::UInt3> {};
template <> struct AttributeDataTypeOf<uint32_t, 4> : std::integral_constant<AttributeDataType, AttributeDataType::UInt4> {};
template <> struct AttributeDataTypeOf<float, 1> : std::integral_constant<AttributeDataType, AttributeDataType::Float> {};
template <> struct AttributeDataTypeOf<float, 2> : std::integral_constant<AttributeDataType, AttributeDataType::Float2> {};
template <> struct AttributeDataTypeOf<float, 3> : std::integral_constant<AttributeDataType, AttributeDataType::Float3> {};
template <> struct AttributeDataTypeOf<float, 4> : std::integral_constant<AttributeDataType, AttributeDataType::Float4> {};

} // namespace

template <typename T, std::size_t N>
class AttributeType {
public:
    using ElementType = T;
    static constexpr AttributeDataType DataType = AttributeDataTypeOf<T, N>::value;
    static constexpr size_t Dimensions = N;
    using Value = std::array<T, N>;
};

struct AttributeDescriptor {
    AttributeDataType dataType;
    uint8_t offset;
};

inline bool operator==(const AttributeDescriptor& lhs, const AttributeDescriptor& rhs) {
    return lhs.dataType == rhs.dataType && lhs.offset == rhs.offset;
}

struct VertexDescriptor {
    uint8_t stride;
    uint8_t count;
    AttributeDescriptor attributes[5];
};

class AttributeBinding {
public:
    AttributeDescriptor attribute;
    uint8_t vertexStride;
    const VertexBufferResource* vertexBufferResource;
    uint32_t vertexOffset;

    friend bool operator==(const AttributeBinding& lhs, const AttributeBinding& rhs) {
        return lhs.attribute == rhs.attribute &&
               lhs.vertexStride == rhs.vertexStride &&
               lhs.vertexBufferResource == rhs.vertexBufferResource &&
               lhs.vertexOffset == rhs.vertexOffset;
    }
};

// Attribute binding requires member offsets. The only standard way to
// obtain an offset is the offsetof macro. The offsetof macro has defined
// behavior only for standard layout types. That rules out std::tuple and
// any other solution that relies on chained inheritance. Manually implemented
// variadic specialization looks like the only solution. Fortunately, we
// only use a maximum of five attributes.

namespace detail {

template <class...>
struct VertexType;

template <class A1>
struct VertexType<A1> {
    using Type = VertexType<A1>;
    typename A1::Value a1;
} MBGL_VERTEX_ALIGN;

template <class A1, class A2>
struct VertexType<A1, A2> {
    using Type = VertexType<A1, A2>;
    typename A1::Value a1;
    typename A2::Value a2;
} MBGL_VERTEX_ALIGN;

template <class A1, class A2, class A3>
struct VertexType<A1, A2, A3> {
    using Type = VertexType<A1, A2, A3>;
    typename A1::Value a1;
    typename A2::Value a2;
    typename A3::Value a3;
} MBGL_VERTEX_ALIGN;

template <class A1, class A2, class A3, class A4>
struct VertexType<A1, A2, A3, A4> {
    using Type = VertexType<A1, A2, A3, A4>;
    typename A1::Value a1;
    typename A2::Value a2;
    typename A3::Value a3;
    typename A4::Value a4;
} MBGL_VERTEX_ALIGN;

template <class A1, class A2, class A3, class A4, class A5>
struct VertexType<A1, A2, A3, A4, A5> {
    using Type = VertexType<A1, A2, A3, A4, A5>;
    typename A1::Value a1;
    typename A2::Value a2;
    typename A3::Value a3;
    typename A4::Value a4;
    typename A5::Value a5;
} MBGL_VERTEX_ALIGN;

template <class>
struct Descriptor;

template <class A1>
struct Descriptor<VertexType<A1>> {
    using Type = VertexType<A1>;
    static_assert(sizeof(Type) < 256, "vertex type must be smaller than 256 bytes");
    static_assert(std::is_standard_layout<Type>::value, "vertex type must use standard layout");
    static constexpr const VertexDescriptor data = { sizeof(Type), 1, {
        { A1::DataType, offsetof(Type, a1) },
    }};
};

template <class A1>
constexpr const VertexDescriptor Descriptor<VertexType<A1>>::data;

template <class A1, class A2>
struct Descriptor<VertexType<A1, A2>> {
    using Type = VertexType<A1, A2>;
    static_assert(sizeof(Type) < 256, "vertex type must be smaller than 256 bytes");
    static_assert(std::is_standard_layout<Type>::value, "vertex type must use standard layout");
    static constexpr const VertexDescriptor data = { sizeof(Type), 2, {
       { A1::DataType, offsetof(Type, a1) },
       { A2::DataType, offsetof(Type, a2) },
    }};
};

template <class A1, class A2>
constexpr const VertexDescriptor Descriptor<VertexType<A1, A2>>::data;

template <class A1, class A2, class A3>
struct Descriptor<VertexType<A1, A2, A3>> {
    using Type = VertexType<A1, A2, A3>;
    static_assert(sizeof(Type) < 256, "vertex type must be smaller than 256 bytes");
    static_assert(std::is_standard_layout<Type>::value, "vertex type must use standard layout");
    static constexpr const VertexDescriptor data = { sizeof(Type), 3, {
        { A1::DataType, offsetof(Type, a1) },
        { A2::DataType, offsetof(Type, a2) },
        { A3::DataType, offsetof(Type, a3) },
    }};
};

template <class A1, class A2, class A3>
constexpr const VertexDescriptor Descriptor<VertexType<A1, A2, A3>>::data;

template <class A1, class A2, class A3, class A4>
struct Descriptor<VertexType<A1, A2, A3, A4>> {
    using Type = VertexType<A1, A2, A3, A4>;
    static_assert(sizeof(Type) < 256, "vertex type must be smaller than 256 bytes");
    static_assert(std::is_standard_layout<Type>::value, "vertex type must use standard layout");
    static constexpr const VertexDescriptor data = { sizeof(Type), 4, {
        { A1::DataType, offsetof(Type, a1) },
        { A2::DataType, offsetof(Type, a2) },
        { A3::DataType, offsetof(Type, a3) },
        { A4::DataType, offsetof(Type, a4) },
    }};
};

template <class A1, class A2, class A3, class A4>
constexpr const VertexDescriptor Descriptor<VertexType<A1, A2, A3, A4>>::data;

template <class A1, class A2, class A3, class A4, class A5>
struct Descriptor<VertexType<A1, A2, A3, A4, A5>> {
    using Type = VertexType<A1, A2, A3, A4, A5>;
    static_assert(sizeof(Type) < 256, "vertex type must be smaller than 256 bytes");
    static_assert(std::is_standard_layout<Type>::value, "vertex type must use standard layout");
    static constexpr const VertexDescriptor data = { sizeof(Type), 5, {
        { A1::DataType, offsetof(Type, a1) },
        { A2::DataType, offsetof(Type, a2) },
        { A3::DataType, offsetof(Type, a3) },
        { A4::DataType, offsetof(Type, a4) },
        { A5::DataType, offsetof(Type, a5) },
    }};
};

template <class A1, class A2, class A3, class A4, class A5>
constexpr const VertexDescriptor Descriptor<VertexType<A1, A2, A3, A4, A5>>::data;

template <class>
struct Vertex;

template <class... As>
struct Vertex<TypeList<As...>> {
    using Type = VertexType<typename As::Type...>;
};

} // namespace detail

template <class A>
using Vertex = typename detail::Vertex<A>::Type;

template <class T>
using VertexType = typename detail::VertexType<T>;

template <size_t I = 0, class... As>
AttributeBinding attributeBinding(const VertexBuffer<detail::VertexType<As...>>& buffer) {
    using Descriptor = detail::Descriptor<detail::VertexType<As...>>;
    static_assert(I < Descriptor::data.count, "attribute index must be in range");
    return {
        Descriptor::data.attributes[I],
        Descriptor::data.stride,
        &buffer.getResource(),
        0,
    };
}

optional<gfx::AttributeBinding> offsetAttributeBinding(const optional<gfx::AttributeBinding>& binding, std::size_t vertexOffset);

template <class>
class AttributeBindings;

template <class... As>
class AttributeBindings<TypeList<As...>> final
    : public IndexedTuple<TypeList<As...>,
                          TypeList<ExpandToType<As, optional<AttributeBinding>>...>> {
    using Base = IndexedTuple<TypeList<As...>,
                          TypeList<ExpandToType<As, optional<AttributeBinding>>...>>;

public:
    AttributeBindings(const VertexBuffer<Vertex<TypeList<As...>>>& buffer)
        : Base{ attributeBinding<TypeIndex<As, As...>::value>(buffer)... } {
    }

    template <class... Args>
    AttributeBindings(Args&&... args) : Base(std::forward<Args>(args)...) {
    }

    AttributeBindings offset(const std::size_t vertexOffset) const {
        return { offsetAttributeBinding(Base::template get<As>(), vertexOffset)... };
    }

    uint32_t activeCount() const {
        uint32_t result = 0;
        util::ignore({ ((result += bool(Base::template get<As>())), 0)... });
        return result;
    }
};

} // namespace gfx
} // namespace mbgl
