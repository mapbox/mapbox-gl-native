#include <llmr/util/raster.hpp>

#include <memory>
#include <cassert>
#include <cstring>

#include <llmr/platform/platform.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/uv.hpp>
#include <llmr/util/std.hpp>

#include <png.h>

using namespace llmr;

Raster::Raster(Texturepool &texturepool)
    : texturepool(texturepool)
{}

Raster::~Raster() {
    if (textured) {
        texturepool.removeTextureID(texture);
    }
}

bool Raster::isLoaded() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

bool Raster::load(const std::string &data) {
    img = std::make_unique<util::Image>(data);
    width = img->getWidth();
    height = img->getHeight();

    std::lock_guard<std::mutex> lock(mtx);
    if (img->getData()) {
        loaded = true;
    }
    return loaded;
}


void Raster::bind(bool linear) {
    if (!width || !height) {
        fprintf(stderr, "trying to bind texture without dimension\n");
        return;
    }

    if (img && !textured) {
        texture = texturepool.getTextureID();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->getData());
        img.reset();
        textured = true;
    } else if (textured) {
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    GLuint filter = linear ? GL_LINEAR : GL_NEAREST;
    if (filter != this->filter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        this->filter = filter;
    }
}

void Raster::beginFadeInTransition() {
    time start = util::now();
    fade_transition = std::make_shared<util::ease_transition>(opacity, 1.0, opacity, start, 250_milliseconds);
}

bool Raster::needsTransition() const {
    return fade_transition != nullptr;
}

void Raster::updateTransitions(time now) {
    if (fade_transition->update(now) == util::transition::complete) {
        fade_transition = nullptr;
    }
}

