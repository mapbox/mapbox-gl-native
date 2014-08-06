#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/platform.hpp>

using namespace mbgl;

LineAtlas::LineAtlas(uint16_t width, uint16_t height)
    : width(width),
      height(height),
      data(new char[width * height]),
      dirty(true) {
}

LineAtlas::~LineAtlas() {
}

void LineAtlas::addDash() {
    for (int i = 0; i < width * height; i++) {
        data[i] = 127;
    }

    dirty = true;
    bind();
};

void LineAtlas::bind() {
    if (!texture) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        dirty = true;
    } else {
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    dirty = true;

    if (dirty) {
        // TODO use texsubimage for updates
        //std::lock_guard<std::mutex> lock(mtx);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
        dirty = false;
    }
};
