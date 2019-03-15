#include <mbgl/gfx/attribute.hpp>

#include <limits>
#include <cassert>

namespace mbgl {
namespace gfx {

optional<AttributeBinding> offsetAttributeBinding(const optional<AttributeBinding>& binding, std::size_t vertexOffset) {
    assert(vertexOffset <= std::numeric_limits<uint32_t>::max());
    if (binding) {
        AttributeBinding result = *binding;
        result.vertexOffset = static_cast<uint32_t>(vertexOffset);
        return result;
    } else {
        return binding;
    }
}

} // namespace gfx
} // namespace mbgl
