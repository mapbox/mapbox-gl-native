#include <llmr/util/raster.hpp>

#include <memory>
#include <cassert>

#include <llmr/platform/platform.hpp>
#include <llmr/platform/gl.hpp>

using namespace llmr;

Raster::~Raster() {
    if (img && !textured) {
        free(img);
    } else if (textured) {
        texturepool->removeTextureID(texture);
    }
}

Raster::operator bool() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

void Raster::load(const std::string& data) {
    std::shared_ptr<Raster> raster = shared_from_this();

    platform::RasterInfo raster_info = platform::load_raster_image(data);

    if (raster_info.img) {
        raster->img = raster_info.img;
        raster->width = raster_info.width;
        raster->height = raster_info.height;
    }

    std::lock_guard<std::mutex> lock(raster->mtx);
    if (raster->img) {
        raster->loaded = true;
    }
}

void Raster::setTexturepool(Texturepool* new_texturepool) {
    texturepool = new_texturepool;
}

void Raster::bind(bool linear) {
    if (!width || !height) {
        fprintf(stderr, "trying to bind texture without dimension\n");
        return;
    }

    if (img && !textured) {
        if (!texturepool) {
            fprintf(stderr, "no available texture pool\n");
            return;
        }
        texture = texturepool->getTextureID();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        free(img);
        img = nullptr;
        textured = true;
    } else {
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    GLuint filter = linear ? GL_LINEAR : GL_NEAREST;
    if (filter != this->filter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        this->filter = filter;
    }
}
