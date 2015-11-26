#include <mbgl/util/image.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/platform/log.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/iostreams/stream.hpp>
#pragma GCC diagnostic pop

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/array.hpp>

extern "C"
{
#include <png.h>
}

namespace mbgl {

using source_type = boost::iostreams::array_source;
using input_stream = boost::iostreams::stream<source_type>;

static void user_error_fn(png_structp, png_const_charp error_msg) {
    throw std::runtime_error(std::string("failed to read invalid png: '") + error_msg + "'");
}

static void user_warning_fn(png_structp, png_const_charp warning_msg) {
    Log::Warning(Event::Image, "ImageReader (PNG): %s", warning_msg);
}

static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
    input_stream * fin = reinterpret_cast<input_stream*>(png_get_io_ptr(png_ptr));
    fin->read(reinterpret_cast<char*>(data), length);
    std::streamsize read_count = fin->gcount();
    if (read_count < 0 || static_cast<png_size_t>(read_count) != length)
    {
        png_error(png_ptr, "Read Error");
    }
}

struct png_struct_guard {
    png_struct_guard(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr)
        : p_(png_ptr_ptr),
          i_(info_ptr_ptr) {}

    ~png_struct_guard() {
        png_destroy_read_struct(p_,i_,0);
    }

    png_structpp p_;
    png_infopp i_;
};

PremultipliedImage decodePNG(const uint8_t* data, size_t size) {
    source_type source(reinterpret_cast<const char*>(data), size);
    input_stream stream(source);

    png_byte header[8] = { 0 };
    stream.read(reinterpret_cast<char*>(header), 8);
    if (stream.gcount() != 8)
        throw std::runtime_error("PNG reader: Could not read image");

    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png)
        throw std::runtime_error("File or stream is not a png");

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr)
        throw std::runtime_error("failed to allocate png_ptr");

    // catch errors in a custom way to avoid the need for setjmp
    png_set_error_fn(png_ptr, png_get_error_ptr(png_ptr), user_error_fn, user_warning_fn);

    png_infop info_ptr;
    png_struct_guard sguard(&png_ptr, &info_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw std::runtime_error("failed to create info_ptr");

    png_set_read_fn(png_ptr, &stream, png_read_data);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    unsigned width = 0;
    unsigned height = 0;
    int bit_depth = 0;
    int color_type = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

    UnassociatedImage image { width, height };

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);

    if (png_get_interlace_type(png_ptr,info_ptr) == PNG_INTERLACE_ADAM7) {
        png_set_interlace_handling(png_ptr); // FIXME: libpng bug?
        // according to docs png_read_image
        // "..automatically handles interlacing,
        // so you don't need to call png_set_interlace_handling()"
    }

    png_read_update_info(png_ptr, info_ptr);

    // we can read whole image at once
    // alloc row pointers
    const std::unique_ptr<png_bytep[]> rows(new png_bytep[height]);
    for (unsigned row = 0; row < height; ++row)
        rows[row] = image.data.get() + row * width * 4;
    png_read_image(png_ptr, rows.get());

    png_read_end(png_ptr, 0);

    return util::premultiply(std::move(image));
}

}
