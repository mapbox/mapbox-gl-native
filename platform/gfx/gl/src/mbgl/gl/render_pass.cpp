#include <mbgl/gl/render_pass.hpp>
#include <mbgl/gl/command_encoder.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {
namespace gl {

RenderPass::RenderPass(gl::CommandEncoder& commandEncoder_,
                       const char* name,
                       const gfx::RenderPassDescriptor& descriptor)
    : commandEncoder(commandEncoder_), debugGroup(commandEncoder.createDebugGroup(name)) {
    descriptor.renderable.getResource<gl::RenderableResource>().bind();
    const auto clearDebugGroup(commandEncoder.createDebugGroup("clear"));
    commandEncoder.context.clear(descriptor.clearColor, descriptor.clearDepth,
                                 descriptor.clearStencil);
}

void RenderPass::pushDebugGroup(const char* name) {
    commandEncoder.pushDebugGroup(name);
}

void RenderPass::popDebugGroup() {
    commandEncoder.popDebugGroup();
}

} // namespace gl
} // namespace mbgl
