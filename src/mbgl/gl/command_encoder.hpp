#pragma once

#include <mbgl/gfx/command_encoder.hpp>

namespace mbgl {
namespace gl {

class Context;

class CommandEncoder : public gfx::CommandEncoder {
public:
    explicit CommandEncoder(gl::Context& context_) : context(context_) {
    }

    ~CommandEncoder() override;

private:
    gl::Context& context;
};

} // namespace gl
} // namespace mbgl
