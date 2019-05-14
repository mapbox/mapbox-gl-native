#pragma once

#include <mbgl/gfx/upload_pass.hpp>

namespace mbgl {
namespace gfx {

class CommandEncoder;

} // namespace gfx

namespace gl {

class CommandEncoder;
class Context;

class UploadPass final : public gfx::UploadPass {
public:
    UploadPass(gl::CommandEncoder&, const char* name);

private:
    void pushDebugGroup(const char* name) override;
    void popDebugGroup() override;

public:
    std::unique_ptr<gfx::VertexBufferResource> createVertexBufferResource(const void* data, std::size_t size, const gfx::BufferUsageType) override;
    void updateVertexBufferResource(gfx::VertexBufferResource&, const void* data, std::size_t size) override;
    std::unique_ptr<gfx::IndexBufferResource> createIndexBufferResource(const void* data, std::size_t size, const gfx::BufferUsageType) override;
    void updateIndexBufferResource(gfx::IndexBufferResource&, const void* data, std::size_t size) override;

public:
    std::unique_ptr<gfx::TextureResource> createTextureResource(Size, const void* data, gfx::TexturePixelType, gfx::TextureChannelDataType) override;
    void updateTextureResource(gfx::TextureResource&, Size, const void* data, gfx::TexturePixelType, gfx::TextureChannelDataType) override;
    void updateTextureResourceSub(gfx::TextureResource&, const uint16_t xOffset, const uint16_t yOffset, Size, const void* data, gfx::TexturePixelType, gfx::TextureChannelDataType) override;

private:
    gl::CommandEncoder& commandEncoder;
    const gfx::DebugGroup<gfx::CommandEncoder> debugGroup;
};

} // namespace gl
} // namespace mbgl
