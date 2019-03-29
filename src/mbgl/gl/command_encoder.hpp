#pragma once

#include <mbgl/gfx/command_encoder.hpp>

namespace mbgl {
namespace gl {

class Context;

class CommandEncoder final : public gfx::CommandEncoder {
public:
    explicit CommandEncoder(gl::Context& context_) : context(context_) {
    }

    ~CommandEncoder() override;

private:
    void pushDebugGroup(const char* name) override;
    void popDebugGroup() override;

public:
    gl::Context& context;
};

} // namespace gl
} // namespace mbgl
