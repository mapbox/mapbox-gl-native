#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/variant.hpp>

#include <cstddef>
#include <functional>

namespace mbgl {
namespace gl {

template <class T, std::size_t N>
class VariableAttributeBinding {
public:
    VariableAttributeBinding(BufferID vertexBuffer_,
                             std::size_t vertexSize_,
                             std::size_t attributeOffset_)
        : vertexBuffer(vertexBuffer_),
          vertexSize(vertexSize_),
          attributeOffset(attributeOffset_)
        {}

    void bind(Context&, AttributeLocation, optional<VariableAttributeBinding<T, N>>&, std::size_t vertexOffset) const;

    friend bool operator==(const VariableAttributeBinding& lhs,
                           const VariableAttributeBinding& rhs) {
        return lhs.vertexBuffer == rhs.vertexBuffer
            && lhs.vertexSize == rhs.vertexSize
            && lhs.attributeOffset == rhs.attributeOffset;
    }

private:
    BufferID vertexBuffer;
    std::size_t vertexSize;
    std::size_t attributeOffset;
};

template <class T, std::size_t N>
class ConstantAttributeBinding {
public:
    ConstantAttributeBinding() { value.fill(T()); }

    explicit ConstantAttributeBinding(std::array<T, N> value_)
        : value(std::move(value_))
        {}

    void bind(Context&, AttributeLocation, optional<VariableAttributeBinding<T, N>>&, std::size_t) const;

    friend bool operator==(const ConstantAttributeBinding& lhs,
                           const ConstantAttributeBinding& rhs) {
        return lhs.value == rhs.value;
    }

private:
    std::array<T, N> value;
};

template <class T, std::size_t N>
class Attribute {
public:
    using Value = std::array<T, N>;

    using VariableBinding = VariableAttributeBinding<T, N>;
    using ConstantBinding = ConstantAttributeBinding<T, N>;

    using Location = AttributeLocation;

    using Binding = variant<
        ConstantBinding,
        VariableBinding>;

    static void bind(Context& context,
                     const Location& location,
                     optional<VariableBinding>& oldBinding,
                     const Binding& newBinding,
                     std::size_t vertexOffset) {
        Binding::visit(newBinding, [&] (const auto& binding) {
            binding.bind(context, location, oldBinding, vertexOffset);
        });
    }
};

#define MBGL_DEFINE_ATTRIBUTE(type_, n_, name_) \
    struct name_ : ::mbgl::gl::Attribute<type_, n_> { static auto name() { return #name_; } }

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

AttributeLocation bindAttributeLocation(ProgramID, AttributeLocation, const char * name);

template <class... As>
class Attributes {
public:
    using Types = TypeList<As...>;
    using Locations = IndexedTuple<
        TypeList<As...>,
        TypeList<typename As::Location...>>;
    using Bindings = IndexedTuple<
        TypeList<As...>,
        TypeList<typename As::Binding...>>;
    using VariableBindings = IndexedTuple<
        TypeList<As...>,
        TypeList<optional<typename As::VariableBinding>...>>;

    using Vertex = detail::Vertex<As...>;

    template <class A>
    static constexpr std::size_t Index = TypeIndex<A, As...>::value;

    static Locations locations(const ProgramID& id) {
        return Locations { bindAttributeLocation(id, Index<As>, As::name())... };
    }

    template <class DrawMode>
    static Bindings allVariableBindings(const VertexBuffer<Vertex, DrawMode>& buffer) {
        static_assert(std::is_standard_layout<Vertex>::value, "vertex type must use standard layout");

        return Bindings {
            typename As::VariableBinding {
                buffer.buffer,
                sizeof(Vertex),
                Vertex::attributeOffsets[Index<As>]
            }...
        };
    }

    static void bind(Context& context,
                     const Locations& locations,
                     VariableBindings& oldBindings,
                     const Bindings& newBindings,
                     std::size_t vertexOffset) {
        util::ignore({ (As::bind(context,
                                 locations.template get<As>(),
                                 oldBindings.template get<As>(),
                                 newBindings.template get<As>(),
                                 vertexOffset), 0)... });
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
