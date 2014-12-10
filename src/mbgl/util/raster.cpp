#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/gl.hpp>

#include <mbgl/util/raster.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>

#include <cassert>
#include <cstring>

using namespace mbgl;

Raster::Raster(TexturePool& texturePool_)
    : texturePool(texturePool_)
{}

Raster::~Raster() {
    if (textured) {
        texturePool.removeTextureID(texture);
    }
}

bool Raster::isLoaded() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

bool Raster::load(const std::string &data) {
    img = util::make_unique<util::Image>(data);
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
        texture = texturePool.getTextureID();
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
#ifndef GL_ES_VERSION_2_0
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->getData()));
        img.reset();
        textured = true;
    } else if (textured) {
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
    }

    GLuint new_filter = linear ? GL_LINEAR : GL_NEAREST;
    if (new_filter != this->filter) {
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, new_filter));
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, new_filter));
        filter = new_filter;
    }
}

// overload ::bind for prerendered raster textures
void Raster::bind(const GLuint custom_texture) {
    if (img && !textured) {
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, custom_texture));
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->getData()));
        img.reset();
        textured = true;
    } else if (textured) {
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, custom_texture));
    }

    GLuint new_filter = GL_LINEAR;
    if (new_filter != this->filter) {
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, new_filter));
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, new_filter));
        filter = new_filter;
    }

}

void Raster::beginFadeInTransition() {
    timestamp start = util::now();
    fade_transition = std::make_shared<util::ease_transition<double>>(opacity, 1.0, opacity, start, 250_milliseconds);
}

bool Raster::needsTransition() const {
    return fade_transition != nullptr;
}

void Raster::updateTransitions(timestamp now) {
    if (fade_transition->update(now) == util::transition::complete) {
        fade_transition = nullptr;
    }
}
