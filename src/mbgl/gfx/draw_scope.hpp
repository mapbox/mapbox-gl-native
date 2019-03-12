#pragma once

#include <memory>

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
    std::unique_ptr<DrawScopeResource> resource;
};

} // namespace gfx
} // namespace mbgl
