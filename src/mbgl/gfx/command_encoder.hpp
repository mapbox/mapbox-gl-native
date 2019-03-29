#pragma once

#include <mbgl/gfx/debug_group.hpp>

namespace mbgl {
namespace gfx {

class CommandEncoder {
protected:
    explicit CommandEncoder() = default;

    friend class DebugGroup<CommandEncoder>;
    virtual void pushDebugGroup(const char* name) = 0;
    virtual void popDebugGroup() = 0;

public:
    virtual ~CommandEncoder() = default;
    CommandEncoder(const CommandEncoder&) = delete;
    CommandEncoder& operator=(const CommandEncoder&) = delete;

    DebugGroup<CommandEncoder> createDebugGroup(const char* name) {
        return { *this, name };
    }
};

} // namespace gfx
} // namespace mbgl
