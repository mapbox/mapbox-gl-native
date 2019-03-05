#pragma once

#include <memory>

namespace mbgl {
namespace gfx {

class VertexBufferResource {
protected:
    VertexBufferResource() = default;
public:
    virtual ~VertexBufferResource() = default;
};

// This class has a template argument that we use to specify the vertex type. It is not used by
// the implementation, but serves type checking purposes during build time.
template <class>
class VertexBuffer {
public:
    std::size_t elements;
    std::unique_ptr<const VertexBufferResource> resource;
};

} // namespace gfx
} // namespace mbgl
