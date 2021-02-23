#pragma once

#include <mbgl/gfx/debug_group.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>

namespace mbgl {
namespace gfx {

class Renderable;

class RenderPassDescriptor {
public:
    Renderable& renderable;
    optional<Color> clearColor;
    optional<float> clearDepth;
    optional<int32_t> clearStencil;
};

class RenderPass {
protected:
    RenderPass() = default;

    friend class DebugGroup<RenderPass>;
    virtual void pushDebugGroup(const char* name) = 0;
    virtual void popDebugGroup() = 0;

public:
    virtual ~RenderPass() = default;
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    DebugGroup<RenderPass> createDebugGroup(const char* name) {
        return { *this, name };
    }
};

} // namespace gfx
} // namespace mbgl
