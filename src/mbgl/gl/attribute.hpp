#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/indexed_tuple.hpp>

#include <cstddef>
#include <functional>

namespace mbgl {
namespace gl {

template <class Tag, class T, std::size_t N>
class Attribute {
public:
    using Type = T[N];

    class State {
    public:
        explicit State(AttributeLocation location_)
            : location(location_) {}

        AttributeLocation location;
        static constexpr std::size_t count = N;
        static constexpr DataType type = DataTypeOf<T>::value;
    };
};

#define MBGL_DEFINE_ATTRIBUTE(type_, n_, name_) \
    struct name_ : ::mbgl::gl::Attribute<name_, type_, n_> { static constexpr auto name = #name_; }

namespace detail {

// Attribute binding requires member offsets. The only standard way to
// obtain an offset is the offsetof macro. The offsetof macro has defined
// behavior only for standard layout types. That rules out std::tuple and
// any other solution that relies on chained inheritance. Manually implemented
// variadic specialization looks like the only solution. Fortunately, we
// only use a maximum of five attributes.

template <class... As>
class Vertex;

template <class A1>
class Vertex<A1> {
public:
    typename A1::Type a1;

    using VertexType = Vertex<A1>;
    static const std::size_t attributeOffsets[1];
};

template <class A1, class A2>
class Vertex<A1, A2> {
public:
    typename A1::Type a1;
    typename A2::Type a2;

    using VertexType = Vertex<A1, A2>;
    static const std::size_t attributeOffsets[2];
};

template <class A1, class A2, class A3>
class Vertex<A1, A2, A3> {
public:
    typename A1::Type a1;
    typename A2::Type a2;
    typename A3::Type a3;

    using VertexType = Vertex<A1, A2, A3>;
    static const std::size_t attributeOffsets[3];
};

template <class A1, class A2, class A3, class A4>
class Vertex<A1, A2, A3, A4> {
public:
    typename A1::Type a1;
    typename A2::Type a2;
    typename A3::Type a3;
    typename A4::Type a4;

    using VertexType = Vertex<A1, A2, A3, A4>;
    static const std::size_t attributeOffsets[4];
};

template <class A1, class A2, class A3, class A4, class A5>
class Vertex<A1, A2, A3, A4, A5> {
public:
    typename A1::Type a1;
    typename A2::Type a2;
    typename A3::Type a3;
    typename A4::Type a4;
    typename A5::Type a5;

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

void bindAttribute(AttributeLocation location,
                   std::size_t count,
                   DataType type,
                   std::size_t vertexSize,
                   std::size_t vertexOffset,
                   std::size_t attributeOffset);

template <class... As>
class Attributes {
public:
    using State = IndexedTuple<TypeList<As...>, TypeList<typename As::State...>>;
    using Vertex = detail::Vertex<As...>;

    template <class A>
    static constexpr std::size_t Index = TypeIndex<A, As...>::value;

    static State state(const ProgramID& id) {
        return State { typename As::State(bindAttributeLocation(id, Index<As>, As::name))... };
    }

    static std::function<void (std::size_t)> binder(const State& state) {
        return [&state] (std::size_t vertexOffset) {
            util::ignore({ (bindAttribute(state.template get<As>().location,
                                          state.template get<As>().count,
                                          state.template get<As>().type,
                                          sizeof(Vertex),
                                          vertexOffset,
                                          Vertex::attributeOffsets[Index<As>]), 0)... });
        };
    }
};

} // namespace gl
} // namespace mbgl
