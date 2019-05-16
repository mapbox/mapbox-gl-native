#pragma once

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/image.hpp>

#include <memory>

namespace mbgl {
namespace gfx {

class HeadlessBackend : public gfx::Renderable {
public:
    // Factory.
    static std::unique_ptr<HeadlessBackend> makeBackend(Size = { 256, 256 }, gfx::ContextMode = gfx::ContextMode::Unique);
    
    HeadlessBackend(Size);
    virtual PremultipliedImage readStillImage() = 0;
    virtual RendererBackend* getRendererBackend() = 0;
    void setSize(Size);
};

} // namespace gfx
} // namespace mbgl
