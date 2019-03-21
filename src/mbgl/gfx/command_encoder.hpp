#pragma once

namespace mbgl {
namespace gfx {

class CommandEncoder {
protected:
    explicit CommandEncoder() = default;

public:
    virtual ~CommandEncoder() = default;
    CommandEncoder(const CommandEncoder&) = delete;
    CommandEncoder& operator=(const CommandEncoder&) = delete;
};

} // namespace gfx
} // namespace mbgl
