#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/size.hpp>

#include <memory>
#include <cassert>

namespace mbgl {
namespace gfx {

class RenderbufferResource {
protected:
    RenderbufferResource() = default;

public:
    virtual ~RenderbufferResource() = default;
};

template <RenderbufferPixelType renderbufferType>
class Renderbuffer final {
public:
    Renderbuffer(const Size size_, std::unique_ptr<RenderbufferResource> resource_)
        : size(size_), resource(std::move(resource_)) {
    }

    void setShouldClear(bool clear) {
        dirty = clear;
    }

    bool needsClearing() const {
        return dirty;
    }

    const Size& getSize() const {
        return size;
    }

    template <typename T = RenderbufferResource>
    T& getResource() const {
        assert(resource);
        return static_cast<T&>(*resource);
    }

protected:
    Size size;
    bool dirty = false;
    std::unique_ptr<RenderbufferResource> resource;
};

} // namespace gfx
} // namespace mbgl
