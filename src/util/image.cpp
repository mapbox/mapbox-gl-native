#include <mbgl/util/image.hpp>

#include <png.h>

#include <cassert>
#include <cstdlib>


std::string mbgl::util::compress_png(int width, int height, void *rgba, bool flip) {
    png_voidp error_ptr = 0;
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, error_ptr, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Couldn't create png_ptr\n");
        return "";
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!png_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)0);
        fprintf(stderr, "Couldn't create info_ptr\n");
        return "";
    }

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    jmp_buf *jmp_context = (jmp_buf *)png_get_error_ptr(png_ptr);
    if (jmp_context) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return "";
    }

    std::string result;
    png_set_write_fn(png_ptr, &result, [](png_structp png_ptr_, png_bytep data, png_size_t length) {
        std::string *out = static_cast<std::string *>(png_get_io_ptr(png_ptr_));
        out->append(reinterpret_cast<char *>(data), length);
    }, NULL);

    struct ptrs {
        ptrs(size_t count) : rows(new png_bytep[count]) {}
        ~ptrs() { delete[] rows; }
        png_bytep *rows = nullptr;
    } pointers(height);

    for (int i = 0; i < height; i++) {
        pointers.rows[flip ? height - 1 - i : i] = (png_bytep)((png_bytep)rgba + width * 4 * i);
    }

    png_set_rows(png_ptr, info_ptr, pointers.rows);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return result;
}


using namespace mbgl::util;


struct Buffer {
    Buffer(const std::string& data_)
    : data(data_.data()), length(data_.size()) {}
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

Image::Image(const std::string &data, bool flip) {
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

        img = static_cast<char*>(::operator new(width * height * 4));

        char *surface = img;
        assert(surface);

        struct ptrs {
            ptrs(size_t count) : rows(new png_bytep[count]) {}
            ~ptrs() { delete[] rows; }
            png_bytep *rows = nullptr;
        } pointers(height);
        for (unsigned i = 0; i < height; ++i) {
            pointers.rows[flip ? height - 1 - i : i] = (png_bytep)(surface + (i * rowbytes));
        }

        // Read image data
        png_read_image(png, pointers.rows);

        png_read_end(png, nullptr);

        png_destroy_read_struct(&png, &info, nullptr);
    } catch (std::exception& e) {
        fprintf(stderr, "loading PNG failed: %s\n", e.what());
        png_destroy_read_struct(&png, &info, nullptr);
        if (img) {
            ::operator delete(img);
            img = nullptr;
        }
        width = 0;
        height = 0;
    }
}

Image::~Image() {
    ::operator delete(img),img = nullptr;
}
