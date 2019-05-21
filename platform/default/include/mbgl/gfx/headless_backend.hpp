#pragma once

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/image.hpp>

#include <memory>

namespace mbgl {
namespace gfx {

// Common headless backend interface, provides HeadlessBackend backend factory
// and enables extending gfx::Renderable with platform specific implementation
// of readStillImage.
class HeadlessBackend : public gfx::Renderable {
public:
    // Factory.
    static std::unique_ptr<HeadlessBackend> make(Size = { 256, 256 }, gfx::ContextMode = gfx::ContextMode::Unique);
    
    virtual PremultipliedImage readStillImage() = 0;
    virtual RendererBackend* getRendererBackend() = 0;
    void setSize(Size);

protected:
    HeadlessBackend(Size);
};

} // namespace gfx
} // namespace mbgl
