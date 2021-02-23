#pragma once

#include <memory>
#include <cassert>

namespace mbgl {
namespace gfx {

class DrawScopeResource {
protected:
    DrawScopeResource() = default;
public:
    virtual ~DrawScopeResource() = default;
};

class DrawScope {
public:
    explicit DrawScope(std::unique_ptr<DrawScopeResource> resource_) : resource(std::move(resource_)) {
    }

    template <typename T = DrawScopeResource>
    T& getResource() const {
        assert(resource);
        return static_cast<T&>(*resource);
    }

protected:
    std::unique_ptr<DrawScopeResource> resource;
};

} // namespace gfx
} // namespace mbgl
