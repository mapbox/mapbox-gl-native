#ifndef LLMR_GEOMETRY_DEBUG_FONT_BUFFER
#define LLMR_GEOMETRY_DEBUG_FONT_BUFFER

#include <vector>
#include <cstdint>

namespace llmr {

class DebugFontBuffer {
public:
    DebugFontBuffer();
    ~DebugFontBuffer();

    void addText(const char *text, double left, double baseline, double scale = 1);
    void clear();

    uint32_t length();
    void bind();

private:
    std::vector<uint16_t> array;
    uint32_t buffer;
    bool dirty;
};

}

#endif
