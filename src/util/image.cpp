#include <llmr/util/image.hpp>
#include <png.h>


std::string llmr::util::compress_png(int width, int height, void *rgba) {
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
    png_set_write_fn(png_ptr, &result, [](png_structp png_ptr, png_bytep data, png_size_t length) {
        std::string *out = static_cast<std::string *>(png_get_io_ptr(png_ptr));
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
