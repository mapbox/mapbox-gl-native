#pragma once

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/backend.hpp>
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
    static std::unique_ptr<HeadlessBackend>
    Create(const Size size = { 256, 256 },
           const gfx::ContextMode contextMode = gfx::ContextMode::Unique) {
        return Backend::Create<HeadlessBackend, Size, gfx::ContextMode>(size, contextMode);
    }

    virtual PremultipliedImage readStillImage() = 0;
    virtual RendererBackend* getRendererBackend() = 0;
    void setSize(Size);

protected:
    HeadlessBackend(Size);
};

} // namespace gfx
} // namespace mbgl
