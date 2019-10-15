#pragma once

namespace mbgl {
namespace gfx {

struct RenderingStats {
    RenderingStats() = default;
    bool isZero() const;

    int numDrawCalls;
    int numActiveTextures;
    int numCreatedTextures;
    int numBuffers;
    int numFrameBuffers;

    int memTextures;
    int memIndexBuffers;
    int memVertexBuffers;
};

} // namespace gfx
} // namespace mbgl