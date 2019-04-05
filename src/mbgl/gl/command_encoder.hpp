#pragma once

#include <mbgl/gfx/command_encoder.hpp>

namespace mbgl {
namespace gl {

class Context;
class RenderPass;

class CommandEncoder final : public gfx::CommandEncoder {
public:
    explicit CommandEncoder(gl::Context& context_) : context(context_) {
    }

    ~CommandEncoder() override;

    friend class RenderPass;

    std::unique_ptr<gfx::RenderPass> createRenderPass(const char* name, const gfx::RenderPassDescriptor&) override;

private:
    void pushDebugGroup(const char* name) override;
    void popDebugGroup() override;

public:
    gl::Context& context;
};

} // namespace gl
} // namespace mbgl
