#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/optional.hpp>

#include <cstddef>
#include <vector>
#include <set>
#include <functional>
#include <string>
#include <array>
#include <limits>

namespace mbgl {
namespace gl {

static constexpr std::size_t MAX_ATTRIBUTES = 8;

template <class> struct DataTypeOf;
template <> struct DataTypeOf< int8_t>  : std::integral_constant<DataType, DataType::Byte> {};
template <> struct DataTypeOf<uint8_t>  : std::integral_constant<DataType, DataType::UnsignedByte> {};
template <> struct DataTypeOf< int16_t> : std::integral_constant<DataType, DataType::Short> {};
template <> struct DataTypeOf<uint16_t> : std::integral_constant<DataType, DataType::UnsignedShort> {};
template <> struct DataTypeOf< int32_t> : std::integral_constant<DataType, DataType::Integer> {};
template <> struct DataTypeOf<uint32_t> : std::integral_constant<DataType, DataType::UnsignedInteger> {};
template <> struct DataTypeOf<float>    : std::integral_constant<DataType, DataType::Float> {};

class AttributeBinding {
public:
    DataType attributeType;
    uint8_t attributeSize;
    uint32_t attributeOffset;

    BufferID vertexBuffer;
    uint32_t vertexSize;
    uint32_t vertexOffset;

    friend bool operator==(const AttributeBinding& lhs,
                           const AttributeBinding& rhs) {
        return std::tie(lhs.attributeType, lhs.attributeSize, lhs.attributeOffset, lhs.vertexBuffer, lhs.vertexSize, lhs.vertexOffset)
            == std::tie(rhs.attributeType, rhs.attributeSize, rhs.attributeOffset, rhs.vertexBuffer, rhs.vertexSize, rhs.vertexOffset);
    }
};

using AttributeBindingArray = std::array<optional<AttributeBinding>, MAX_ATTRIBUTES>;

/*
    gl::Attribute<T,N> manages the binding of a vertex buffer to a GL program attribute.
      - T is the underlying primitive type (exposed as Attribute<T,N>::ValueType)
      - N is the number of components in the attribute declared in the shader (exposed as Attribute<T,N>::Dimensions)
*/
template <class T, std::size_t N>
class Attribute {
public:
    using ValueType = T;
    static constexpr size_t Dimensions = N;
    using Value = std::array<T, N>;

    using Location = AttributeLocation;
    using Binding = AttributeBinding;

    /*
        Create a binding for this attribute.  The `attributeSize` parameter may be used to
        override the number of components available in the buffer for each vertex.  Thus,
        a buffer with only one float for each vertex can be bound to a `vec2` attribute
    */
    template <class Vertex, class DrawMode>
    static Binding binding(const VertexBuffer<Vertex, DrawMode>& buffer,
                           std::size_t attributeIndex,
                           std::size_t attributeSize = N) {
        static_assert(std::is_standard_layout<Vertex>::value, "vertex type must use standard layout");
        assert(attributeSize >= 1);
        assert(attributeSize <= 4);
        assert(Vertex::attributeOffsets[attributeIndex] <= std::numeric_limits<uint32_t>::max());
        static_assert(sizeof(Vertex) <= std::numeric_limits<uint32_t>::max(), "vertex too large");
        return AttributeBinding {
            DataTypeOf<T>::value,
            static_cast<uint8_t>(attributeSize),
            static_cast<uint32_t>(Vertex::attributeOffsets[attributeIndex]),
            buffer.buffer,
            static_cast<uint32_t>(sizeof(Vertex)),
            0,
        };
    }

    static optional<Binding> offsetBinding(const optional<Binding>& binding, std::size_t vertexOffset) {
        assert(vertexOffset <= std::numeric_limits<uint32_t>::max());
        if (binding) {
            AttributeBinding result = *binding;
            result.vertexOffset = static_cast<uint32_t>(vertexOffset);
            return result;
        } else {
            return binding;
        }
    }
};

#define MBGL_DEFINE_ATTRIBUTE(type_, n_, name_)        \
    struct name_ {                                     \
        static auto name() { return #name_; }          \
        using Type = ::mbgl::gl::Attribute<type_, n_>; \
    }

namespace detail {

// Attribute binding requires member offsets. The only standard way to
// obtain an offset is the offsetof macro. The offsetof macro has defined
// behavior only for standard layout types. That rules out std::tuple and
// any other solution that relies on chained inheritance. Manually implemented
// variadic specialization looks like the only solution. Fortunately, we
// only use a maximum of five attributes.

template <class... As>
class Vertex;

template <>
class Vertex<> {
public:
    using VertexType = Vertex<>;
};

template <class A1>
class Vertex<A1> {
public:
    typename A1::Value a1;

    using VertexType = Vertex<A1>;
    static const std::size_t attributeOffsets[1];
};

template <class A1, class A2>
class Vertex<A1, A2> {
public:
    typename A1::Value a1;
    typename A2::Value a2;

    using VertexType = Vertex<A1, A2>;
    static const std::size_t attributeOffsets[2];
};

template <class A1, class A2, class A3>
class Vertex<A1, A2, A3> {
public:
    typename A1::Value a1;
    typename A2::Value a2;
    typename A3::Value a3;

    using VertexType = Vertex<A1, A2, A3>;
    static const std::size_t attributeOffsets[3];
};

template <class A1, class A2, class A3, class A4>
class Vertex<A1, A2, A3, A4> {
public:
    typename A1::Value a1;
    typename A2::Value a2;
    typename A3::Value a3;
    typename A4::Value a4;

    using VertexType = Vertex<A1, A2, A3, A4>;
    static const std::size_t attributeOffsets[4];
};

template <class A1, class A2, class A3, class A4, class A5>
class Vertex<A1, A2, A3, A4, A5> {
public:
    typename A1::Value a1;
    typename A2::Value a2;
    typename A3::Value a3;
    typename A4::Value a4;
    typename A5::Value a5;

    using VertexType = Vertex<A1, A2, A3, A4, A5>;
    static const std::size_t attributeOffsets[5];
};

template <class A1>
const std::size_t Vertex<A1>::attributeOffsets[1] = {
    offsetof(VertexType, a1)
};

template <class A1, class A2>
const std::size_t Vertex<A1, A2>::attributeOffsets[2] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2)
};

template <class A1, class A2, class A3>
const std::size_t Vertex<A1, A2, A3>::attributeOffsets[3] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2),
    offsetof(VertexType, a3)
};

template <class A1, class A2, class A3, class A4>
const std::size_t Vertex<A1, A2, A3, A4>::attributeOffsets[4] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2),
    offsetof(VertexType, a3),
    offsetof(VertexType, a4)
};

template <class A1, class A2, class A3, class A4, class A5>
const std::size_t Vertex<A1, A2, A3, A4, A5>::attributeOffsets[5] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2),
    offsetof(VertexType, a3),
    offsetof(VertexType, a4),
    offsetof(VertexType, a5)
};

} // namespace detail

void bindAttributeLocation(ProgramID, AttributeLocation, const char * name);
std::set<std::string> getActiveAttributes(ProgramID);

template <class... As>
class Attributes {
public:
    using Types = TypeList<As...>;
    using Locations = IndexedTuple<
        TypeList<As...>,
        TypeList<optional<typename As::Type::Location>...>>;
    using Bindings = IndexedTuple<
        TypeList<As...>,
        TypeList<optional<typename As::Type::Binding>...>>;
    using NamedLocations = std::vector<std::pair<const std::string, AttributeLocation>>;

    using Vertex = detail::Vertex<typename As::Type...>;

    static Locations bindLocations(const ProgramID& id) {
        std::set<std::string> activeAttributes = getActiveAttributes(id);

        AttributeLocation location = 0;
        auto maybeBindLocation = [&](const char* name) -> optional<AttributeLocation> {
            if (activeAttributes.count(name)) {
                bindAttributeLocation(id, location, name);
                return location++;
            } else {
                return {};
            }
        };

        return Locations { maybeBindLocation(As::name())... };
    }

    template <class Program>
    static Locations loadNamedLocations(const Program& program) {
        return Locations{ program.attributeLocation(As::name())... };
    }

    static NamedLocations getNamedLocations(const Locations& locations) {
        NamedLocations result;

        auto maybeAddLocation = [&] (const std::string& name, const optional<AttributeLocation>& location) {
            if (location) {
                result.emplace_back(name, *location);
            }
        };

        util::ignore({ (maybeAddLocation(As::name(), locations.template get<As>()), 0)... });

        return result;
    }

    template <class DrawMode>
    static Bindings bindings(const VertexBuffer<Vertex, DrawMode>& buffer) {
        return Bindings { As::Type::binding(buffer, TypeIndex<As, As...>::value)... };
    }

    static Bindings offsetBindings(const Bindings& bindings, std::size_t vertexOffset) {
        return Bindings { As::Type::offsetBinding(bindings.template get<As>(), vertexOffset)... };
    }

    static AttributeBindingArray toBindingArray(const Locations& locations, const Bindings& bindings) {
        AttributeBindingArray result;

        auto maybeAddBinding = [&] (const optional<AttributeLocation>& location,
                                    const optional<AttributeBinding>& binding) {
            if (location) {
                result.at(*location) = binding;
            }
        };

        util::ignore({ (maybeAddBinding(locations.template get<As>(), bindings.template get<As>()), 0)... });

        return result;
    }
};

namespace detail {

template <class...>
struct ConcatenateAttributes;

template <class... As, class... Bs>
struct ConcatenateAttributes<TypeList<As...>, TypeList<Bs...>> {
    using Type = Attributes<As..., Bs...>;
};

} // namespace detail

template <class A, class B>
using ConcatenateAttributes = typename detail::ConcatenateAttributes<
    typename A::Types,
    typename B::Types>::Type;

} // namespace gl
} // namespace mbgl
