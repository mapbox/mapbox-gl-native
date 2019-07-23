#pragma once

#include <mbgl/gfx/renderbuffer.hpp>
#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class RenderbufferResource final : public gfx::RenderbufferResource {
public:
    explicit RenderbufferResource(UniqueRenderbuffer renderbuffer_)
        : renderbuffer(std::move(renderbuffer_)) {
    }

    UniqueRenderbuffer renderbuffer;
};

} // namespace gl
} // namespace mbgl
