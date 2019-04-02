#pragma once

#include <mbgl/gfx/debug_group.hpp>

#include <memory>

namespace mbgl {
namespace gfx {

class RenderPassDescriptor;
class RenderPass;

class CommandEncoder {
protected:
    explicit CommandEncoder() = default;

    friend class DebugGroup<CommandEncoder>;
    virtual void pushDebugGroup(const char* name) = 0;
    virtual void popDebugGroup() = 0;

public:
    virtual ~CommandEncoder() = default;
    CommandEncoder(const CommandEncoder&) = delete;
    CommandEncoder& operator=(const CommandEncoder&) = delete;

    DebugGroup<CommandEncoder> createDebugGroup(const char* name) {
        return { *this, name };
    }

    virtual std::unique_ptr<RenderPass> createRenderPass(const char* name, const RenderPassDescriptor&) = 0;
};

} // namespace gfx
} // namespace mbgl
