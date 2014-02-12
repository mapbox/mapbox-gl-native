#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/platform/gl.hpp>
#include <cmath>
#include <cstring>

#include "debug_font.cpp"

using namespace llmr;

void DebugFontBuffer::addText(const char *text, double left, double baseline, double scale) {
    uint16_t *coords = nullptr;

    int32_t length = strlen(text);
    for (int32_t i = 0; i < length; ++i) {
        if (text[i] < 32 || text[i] > 127) {
            continue;
        }

        auto it = simplex.find((int)text[i]);
        if (it == simplex.end()) {
            continue;
        }

        const glyph& glyph = it->second;
        if (!glyph.width) {
            continue;
        }

        int16_t prev_x = -1, prev_y = -1, prev = false;
        for (int32_t j = 0; j < glyph.length; j += 2) {
            if (glyph.data[j] == -1 && glyph.data[j + 1] == -1) {
                prev = false;
            } else {
                int16_t x = round(left + glyph.data[j] * scale);
                int16_t y = round(baseline - glyph.data[j + 1] * scale);
                if (prev) {
                    coords = static_cast<uint16_t *>(addElement());
                    coords[0] = prev_x;
                    coords[1] = prev_y;

                    coords = static_cast<uint16_t *>(addElement());
                    coords[0] = x;
                    coords[1] = y;
                }
                prev_x = x; prev_y = y; prev = true;
            }
        }
        left += glyph.width * scale;
    }
}
