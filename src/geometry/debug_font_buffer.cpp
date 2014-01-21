#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/platform/gl.hpp>
#include <cmath>

#include "debug_font.cpp"

using namespace llmr;

debug_font_buffer::debug_font_buffer()
    : buffer(0),
      dirty(true) {

}

debug_font_buffer::~debug_font_buffer() {
    if (buffer != 0) {
        glDeleteBuffers(1, &buffer);
    }
}

void debug_font_buffer::addText(const char *text, double left, double baseline, double scale) {
    int32_t length = strlen(text);
    for (int32_t i = 0; i < length; i++) {
        if (text[i] < 32 || text[i] > 127) {
            continue;
        }
        glyph &glyph = simplex[(int)text[i]];
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
                    array.push_back(prev_x);
                    array.push_back(prev_y);
                    array.push_back(x);
                    array.push_back(y);
                }
                prev_x = x; prev_y = y; prev = true;
            }
        }
        left += glyph.width * scale;
    }

    dirty = true;
}

void debug_font_buffer::clear() {
    array.clear();
    dirty = true;
}

uint32_t debug_font_buffer::length() {
    // We store 2 coordinates per vertex
    return array.size() / 2;
}

void debug_font_buffer::bind() {
    if (buffer == 0) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }

    if (dirty) {
        glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(uint16_t), array.data(), GL_STATIC_DRAW);
        dirty = false;
    }
}
