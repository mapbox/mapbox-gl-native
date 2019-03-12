#pragma once

#include <mbgl/gfx/attribute.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
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

using AttributeBindingArray = std::vector<optional<gfx::AttributeBinding>>;

optional<gfx::AttributeBinding> offsetAttributeBinding(const optional<gfx::AttributeBinding>& binding, std::size_t vertexOffset);

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
    using Bindings = gfx::AttributeBindings<TypeList<As...>>;
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

    static Bindings bindings(const gfx::VertexBuffer<gfx::Vertex<Types>>& buffer) {
        return Bindings { gfx::attributeBinding<TypeIndex<As, As...>::value>(buffer)... };
    }

    static Bindings offsetBindings(const Bindings& bindings, std::size_t vertexOffset) {
        return Bindings { offsetAttributeBinding(bindings.template get<As>(), vertexOffset)... };
    }

    static AttributeBindingArray toBindingArray(const Locations& locations, const Bindings& bindings) {
        AttributeBindingArray result;
        result.resize(sizeof...(As));

        auto maybeAddBinding = [&] (const optional<AttributeLocation>& location,
                                    const optional<gfx::AttributeBinding>& binding) {
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
