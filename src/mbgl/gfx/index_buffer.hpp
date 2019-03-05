#pragma once

#include <memory>

namespace mbgl {
namespace gfx {

class IndexBufferResource {
protected:
    IndexBufferResource() = default;
public:
    virtual ~IndexBufferResource() = default;
};

class IndexBuffer {
public:
    std::size_t elements;
    std::unique_ptr<const IndexBufferResource> resource;
};

} // namespace gfx
} // namespace mbgl
