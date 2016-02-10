#include <mbgl/platform/platform.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/raster.hpp>

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

void Raster::load(PremultipliedImage image) {
    assert(image.data.get());

    img = std::move(image);
    width = GLsizei(img.width);
    height = GLsizei(img.height);

    std::lock_guard<std::mutex> lock(mtx);
    loaded = true;
}


void Raster::bind(bool linear) {
    if (!width || !height) {
        Log::Error(Event::OpenGL, "trying to bind texture without dimension");
        return;
    }

    if (img.data && !textured) {
        upload();
    } else if (textured) {
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
    }

    GLint new_filter = linear ? GL_LINEAR : GL_NEAREST;
    if (new_filter != this->filter) {
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, new_filter));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, new_filter));
        filter = new_filter;
    }
}

void Raster::upload() {
    if (img.data && !textured) {
        texture = texturePool.getTextureID();
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
#ifndef GL_ES_VERSION_2_0
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data.get()));
        img.data.reset();
        textured = true;
    }
}
