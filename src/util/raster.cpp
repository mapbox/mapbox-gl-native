#include <llmr/util/raster.hpp>

#include <memory>
#include <cassert>
#include <cstring>

#include <llmr/platform/platform.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/uv.hpp>

#include <png.h>

using namespace llmr;

Raster::~Raster() {
    if (img && !textured) {
        free(img);
    } else if (textured) {
        texturepool->removeTextureID(texture);
    }
}

bool Raster::isLoaded() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

void Raster::load() {
    loadImage(data);

    {
        std::lock_guard<std::mutex> lock(mtx);
        if (img) {
            loaded = true;
        }
    }

    data.clear();
}

struct Buffer {
    Buffer(const std::string& data)
    : data(data.data()), length(data.size()) {}
    const char *const data = 0;
    const size_t length = 0;
    size_t pos = 0;
};

void readCallback(png_structp png, png_bytep data, png_size_t length) {
    Buffer *reader = static_cast<Buffer *>(png_get_io_ptr(png));

    // Read `length` bytes into `data`.
    if (reader->pos + length > reader->length) {
        png_error(png, "Read Error");
    } else {
        memcpy(data, reader->data + reader->pos, length);
        reader->pos += length;
    }
}

void errorHandler(png_structp, png_const_charp error_msg) {
    throw std::runtime_error(error_msg);
}

void warningHandler(png_structp, png_const_charp error_msg) {
    fprintf(stderr, "PNG: %s\n", error_msg);
}

void Raster::loadImage(const std::string& data) {
    std::lock_guard<std::mutex> lock(mtx);

    Buffer buffer(data);

    if (buffer.length < 8 || !png_check_sig((const png_bytep)buffer.data, 8)) {
        fprintf(stderr, "image is not a valid PNG image\n");
        return;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, errorHandler, warningHandler);
    assert(png);

    png_infop info = png_create_info_struct(png);
    assert(info);

    int depth, color, interlace;

    try {
        png_set_read_fn(png, (png_voidp)&buffer, readCallback);
        png_read_info(png, info);
        png_get_IHDR(png, info, (png_uint_32*)&width, (png_uint_32*)&height, &depth, &color, &interlace, nullptr, nullptr);
        bool alpha = (color & PNG_COLOR_MASK_ALPHA) || png_get_valid(png, info, PNG_INFO_tRNS);

        // From http://trac.mapnik.org/browser/trunk/src/png_reader.cpp
        if (color == PNG_COLOR_TYPE_PALETTE)
            png_set_expand(png);
        if (color == PNG_COLOR_TYPE_GRAY)
            png_set_expand(png);
        if (png_get_valid(png, info, PNG_INFO_tRNS))
            png_set_expand(png);
        if (depth == 16)
            png_set_strip_16(png);
        if (depth < 8)
            png_set_packing(png);
        if (color == PNG_COLOR_TYPE_GRAY ||
            color == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);

        if (interlace == PNG_INTERLACE_ADAM7)
            png_set_interlace_handling(png);

        // Always add an alpha channel.
        if (!alpha) {
            png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
        }

        double gamma;
        if (png_get_gAMA(png, info, &gamma))
            png_set_gamma(png, 2.2, gamma);

        png_read_update_info(png, info);

        png_size_t rowbytes = png_get_rowbytes(png, info);
        assert(width * 4 == rowbytes);

        img = (char *)malloc(width * height * 4);
        char *surface = img;
        assert(surface);

        struct ptrs {
            ptrs(size_t count) : rows(new png_bytep[count]) {}
            ~ptrs() { delete[] rows; }
            png_bytep *rows = nullptr;
        } pointers(height);
        for (unsigned i = 0; i < height; ++i) {
            pointers.rows[i] = (png_bytep)(surface + (i * rowbytes));
        }

        // Read image data
        png_read_image(png, pointers.rows);

        png_read_end(png, nullptr);

        png_destroy_read_struct(&png, &info, nullptr);
    } catch (std::exception& e) {
        fprintf(stderr, "loading PNG failed: %s\n", e.what());
        png_destroy_read_struct(&png, &info, nullptr);
        if (img) {
            free(img);
            img = nullptr;
        }
        width = 0;
        height = 0;
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

    if (img && !textured && texturepool) {
        texture = texturepool->getTextureID();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        free(img);
        img = nullptr;
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

void Raster::beginFadeInAnimation() {
    time start = util::now();
    fade_animation = std::make_shared<util::ease_animation>(opacity, 1.0, opacity, start, 250_milliseconds);
}

bool Raster::needsAnimation() const {
    return fade_animation != nullptr;
}

void Raster::updateAnimations(time now) {
    if (fade_animation->update(now) == util::animation::complete) {
        fade_animation = nullptr;
    }
}

