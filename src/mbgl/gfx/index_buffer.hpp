#pragma once

#include <memory>
#include <cassert>

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
    IndexBuffer(const std::size_t elements_, std::unique_ptr<IndexBufferResource>&& resource_)
        : elements(elements_), resource(std::move(resource_)) {
    }

    std::size_t elements;

    template <typename T = IndexBufferResource>
    T& getResource() const {
        assert(resource);
        return static_cast<T&>(*resource);
    }

protected:
    std::unique_ptr<IndexBufferResource> resource;
};

} // namespace gfx
} // namespace mbgl
