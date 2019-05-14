#pragma once

#include <mbgl/util/size.hpp>

#include <memory>
#include <cassert>

namespace mbgl {
namespace gfx {

class RenderableResource {
protected:
    explicit RenderableResource() = default;

public:
    virtual ~RenderableResource() = default;
    RenderableResource(const RenderableResource&) = delete;
    RenderableResource& operator=(const RenderableResource&) = delete;
};

class Renderable {
protected:
    Renderable(const Size size_, std::unique_ptr<RenderableResource> resource_)
        : size(size_), resource(std::move(resource_)) {
    }
    virtual ~Renderable() = default;

public:
    Size getSize() const {
        return size;
    }

    template <typename T>
    T& getResource() const {
        assert(resource);
        return static_cast<T&>(*resource);
    }

    virtual void wait() {}

protected:
    Size size;
    std::unique_ptr<RenderableResource> resource;
};

} // namespace gfx
} // namespace mbgl
