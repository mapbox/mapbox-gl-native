#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/debug_font_data.hpp>

#include <mbgl/platform/gl.hpp>
#include <cmath>
#include <cstring>

using namespace mbgl;

void DebugFontBuffer::addText(const char *text, double left, double baseline, double scale) {
    uint16_t *coords = nullptr;

    const size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        if (text[i] < 32 || (unsigned char)(text[i]) >= 127) {
            continue;
        }

        const glyph& glyph = simplex[text[i] - 32];

        int16_t prev_x = -1, prev_y = -1, prev = false;
        for (int32_t j = 0; j < glyph.length; j += 2) {
            if (glyph.data[j] == -1 && glyph.data[j + 1] == -1) {
                prev = false;
            } else {
                int16_t x = ::round(left + glyph.data[j] * scale);
                int16_t y = ::round(baseline - glyph.data[j + 1] * scale);
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
