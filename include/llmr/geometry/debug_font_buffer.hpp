#ifndef LLMR_GEOMETRY_DEBUG_FONT_BUFFER
#define LLMR_GEOMETRY_DEBUG_FONT_BUFFER

#include "buffer.hpp"

namespace llmr {

class DebugFontBuffer : public Buffer<
    4 // 2 bytes per coordinate, 2 coordinates
> {
public:
    void addText(const char *text, double left, double baseline, double scale = 1);
};

}

#endif
