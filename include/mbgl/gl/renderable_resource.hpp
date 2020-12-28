#pragma once

#include <mbgl/gfx/renderable.hpp>

#include <cstdlib>

namespace mbgl {
namespace gl {

class RenderableResource : public gfx::RenderableResource {
protected:
    explicit RenderableResource() = default;

public:
    virtual void bind() = 0;

    virtual void swap() {
        // Renderable resources that require a swap function to be called explicitly
        // can override this method.
    }
};

} // namespace gl
} // namespace mbgl
