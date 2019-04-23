#pragma once

#include <mbgl/gfx/attribute.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/util/literal.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <string>
#include <array>

namespace mbgl {
namespace gl {

using AttributeBindingArray = std::vector<optional<gfx::AttributeBinding>>;
using NamedAttributeLocations = std::vector<std::pair<const std::string, AttributeLocation>>;

optional<AttributeLocation> queryLocation(ProgramID id, const char* name);

template <class>
class AttributeLocations;

template <class... As>
class AttributeLocations<TypeList<As...>> final {
private:
    using Locations =
        IndexedTuple<TypeList<As...>, TypeList<ExpandToType<As, optional<AttributeLocation>>...>>;

    Locations locations;

public:
    AttributeLocations() = default;

    template <class BinaryProgram>
    AttributeLocations(const BinaryProgram& program)
        : locations{ program.attributeLocation(
              concat_literals<&string_literal<'a', '_'>::value, &As::name>::value())... } {
    }

    void queryLocations(const ProgramID& id) {
        locations = Locations{
            queryLocation(id, concat_literals<&string_literal<'a', '_'>::value, &As::name>::value())... };
    }
    
    NamedAttributeLocations getNamedLocations() const {
        NamedAttributeLocations result;

        auto maybeAddLocation = [&] (const std::string& name, const optional<AttributeLocation>& location) {
            if (location) {
                result.emplace_back(name, *location);
            }
        };

        util::ignore({ (maybeAddLocation(concat_literals<&string_literal<'a', '_'>::value, &As::name>::value(),
                                         locations.template get<As>()),
                        0)... });

        return result;
    }

    AttributeBindingArray toBindingArray(const gfx::AttributeBindings<TypeList<As...>>& bindings) const {
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
};

template <class>
class AttributeKey;

constexpr auto attributeDefinePrefix() {
    return "#define HAS_UNIFORM_u_";
}

template <class... As>
class AttributeKey<TypeList<As...>> final {
public:
    static_assert(sizeof...(As) <= 32, "attribute count exceeds 32");

    static uint32_t compute(const gfx::AttributeBindings<TypeList<As...>>& bindings) {
        uint32_t value = 0;
        util::ignore(
            { (bindings.template get<As>() ? (void)(value |= 1 << TypeIndex<As, As...>::value)
                                           : (void)0,
               0)... });
        return value;
    }

    static std::string defines(const gfx::AttributeBindings<TypeList<As...>>& bindings) {
        std::string result;
        util::ignore({ (!bindings.template get<As>()
                            ? (void)(result += concat_literals<&attributeDefinePrefix, &As::name, &string_literal<'\n'>::value>::value())
                            : (void)0,
                        0)... });
        return result;
    }
};

} // namespace gl
} // namespace mbgl
