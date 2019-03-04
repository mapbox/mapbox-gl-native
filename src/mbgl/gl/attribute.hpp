#pragma once

#include <mbgl/gfx/attribute.hpp>
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

class AttributeBinding {
public:
    gfx::AttributeDescriptor attribute;
    uint8_t vertexStride;
    BufferID vertexBuffer;
    uint32_t vertexOffset;

    friend bool operator==(const AttributeBinding& lhs,
                           const AttributeBinding& rhs) {
        return std::tie(lhs.attribute, lhs.vertexStride, lhs.vertexBuffer, lhs.vertexOffset)
            == std::tie(rhs.attribute, rhs.vertexStride, rhs.vertexBuffer, rhs.vertexOffset);
    }
};

using AttributeBindingArray = std::vector<optional<AttributeBinding>>;

    /*
        Create a binding for this attribute.  The `attributeSize` parameter may be used to
        override the number of components available in the buffer for each vertex.  Thus,
        a buffer with only one float for each vertex can be bound to a `vec2` attribute
    */
template <std::size_t I, typename Vertex>
AttributeBinding attributeBinding(const VertexBuffer<Vertex>& buffer) {
    static_assert(I < gfx::VertexDescriptorOf<Vertex>::data.count, "vertex attribute index out of range");
    return {
        gfx::VertexDescriptorOf<Vertex>::data.attributes[I],
        gfx::VertexDescriptorOf<Vertex>::data.stride,
        buffer.buffer,
        0,
    };
}

optional<AttributeBinding> offsetAttributeBinding(const optional<AttributeBinding>& binding, std::size_t vertexOffset);

class Context;
void bindAttributeLocation(Context&, ProgramID, AttributeLocation, const char * name);
std::set<std::string> getActiveAttributes(ProgramID);

template <class>
class Attributes;

template <class... As>
class Attributes<TypeList<As...>> final {
public:
    using Types = TypeList<As...>;
    using Locations = IndexedTuple<
        TypeList<As...>,
        TypeList<ExpandToType<As, optional<AttributeLocation>>...>>;
    using Bindings = IndexedTuple<
        TypeList<As...>,
        TypeList<ExpandToType<As, optional<AttributeBinding>>...>>;
    using NamedLocations = std::vector<std::pair<const std::string, AttributeLocation>>;

    static Locations bindLocations(Context& context, const ProgramID& id) {
        std::set<std::string> activeAttributes = getActiveAttributes(id);

        AttributeLocation location = 0;
        auto maybeBindLocation = [&](const char* name) -> optional<AttributeLocation> {
            if (activeAttributes.count(name)) {
                bindAttributeLocation(context, id, location, name);
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

    static Bindings bindings(const VertexBuffer<gfx::Vertex<Types>>& buffer) {
        return Bindings { attributeBinding<TypeIndex<As, As...>::value>(buffer)... };
    }

    static Bindings offsetBindings(const Bindings& bindings, std::size_t vertexOffset) {
        return Bindings { offsetAttributeBinding(bindings.template get<As>(), vertexOffset)... };
    }

    static AttributeBindingArray toBindingArray(const Locations& locations, const Bindings& bindings) {
        AttributeBindingArray result;
        result.resize(sizeof...(As));

        auto maybeAddBinding = [&] (const optional<AttributeLocation>& location,
                                    const optional<AttributeBinding>& binding) {
            if (location) {
                result.at(*location) = binding;
            }
        };

        util::ignore({ (maybeAddBinding(locations.template get<As>(), bindings.template get<As>()), 0)... });

        return result;
    }

    static uint32_t activeBindingCount(const Bindings& bindings) {
        uint32_t result = 0;
        util::ignore({ ((result += bool(bindings.template get<As>())), 0)... });
        return result;
    }
};

} // namespace gl
} // namespace mbgl
