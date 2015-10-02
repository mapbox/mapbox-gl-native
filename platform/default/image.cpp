#include <mbgl/util/image.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>

#include <png.h>

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <cstring>

#include <mbgl/platform/default/image_reader.hpp>

// Check png library version.
const static bool png_version_check = []() {
    const png_uint_32 version = png_access_version_number();
    if (version != PNG_LIBPNG_VER) {
        throw std::runtime_error(mbgl::util::sprintf<96>(
            "libpng version mismatch: headers report %d.%d.%d, but library reports %d.%d.%d",
            PNG_LIBPNG_VER / 10000, (PNG_LIBPNG_VER / 100) % 100, PNG_LIBPNG_VER % 100,
            version / 10000, (version / 100) % 100, version % 100));
    }
    return true;
}();


namespace mbgl {
namespace util {

std::string compress_png(int width, int height, const void *rgba) {
    png_voidp error_ptr = 0;
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, error_ptr, NULL, NULL);
    if (!png_ptr) {
        Log::Error(Event::Image, "couldn't create png_ptr");
        return "";
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!png_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)0);
        Log::Error(Event::Image, "couldn't create info_ptr");
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
        pointers.rows[i] = (png_bytep)((png_bytep)rgba + width * 4 * i);
    }

    png_set_rows(png_ptr, info_ptr, pointers.rows);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return result;
}

Image::Image(std::string const& data)
{
    try
    {
        auto reader = getImageReader(data.c_str(), data.size());
        width = reader->width();
        height = reader->height();
        img = std::make_unique<char[]>(width * height * 4);
        reader->read(0, 0, width, height, img.get());
    }
    catch (ImageReaderException const& ex)
    {
        Log::Error(Event::Image, "%s", ex.what());
        img.reset();
        width = 0;
        height = 0;

    }
    catch (...) // catch the rest
    {
        Log::Error(Event::Image, "exception in constructor");
        img.reset();
        width = 0;
        height = 0;
    }
}

}
}
