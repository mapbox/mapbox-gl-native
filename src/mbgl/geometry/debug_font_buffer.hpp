#pragma once

#include <mbgl/geometry/buffer.hpp>

namespace mbgl {

class DebugFontBuffer : public Buffer<4 // 2 bytes per coordinate, 2 coordinates
                                      > {
public:
    void addText(const char* text, double left, double baseline, double scale = 1);
};

} // namespace mbgl
