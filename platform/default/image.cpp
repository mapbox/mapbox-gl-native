#include <mbgl/util/image.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/premultiply.hpp>

#include <png.h>

template<size_t max, typename... Args>
static std::string sprintf(const char *msg, Args... args) {
    char res[max];
    int len = snprintf(res, sizeof(res), msg, args...);
    return std::string(res, len);
}

const static bool png_version_check __attribute__((unused)) = []() {
    const png_uint_32 version = png_access_version_number();
    if (version != PNG_LIBPNG_VER) {
        throw std::runtime_error(sprintf<96>(
            "libpng version mismatch: headers report %d.%d.%d, but library reports %d.%d.%d",
            PNG_LIBPNG_VER / 10000, (PNG_LIBPNG_VER / 100) % 100, PNG_LIBPNG_VER % 100,
            version / 10000, (version / 100) % 100, version % 100));
    }
    return true;
}();

namespace mbgl {

std::string encodePNG(const PremultipliedImage& pre) {
    PremultipliedImage copy(pre.size);
    std::copy(pre.data.get(), pre.data.get() + pre.bytes(), copy.data.get());

    UnassociatedImage src = util::unpremultiply(std::move(copy));

    png_voidp error_ptr = nullptr;
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, error_ptr, nullptr, nullptr);
    if (!png_ptr) {
        throw std::runtime_error("couldn't create png_ptr");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!png_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
        throw std::runtime_error("couldn't create info_ptr");
    }

    png_set_IHDR(png_ptr, info_ptr, src.size.width, src.size.height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    jmp_buf *jmp_context = (jmp_buf *)png_get_error_ptr(png_ptr);
    if (jmp_context) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        throw std::runtime_error("png error");
    }

    std::string result;
    png_set_write_fn(png_ptr, &result, [](png_structp png_ptr_, png_bytep data, png_size_t length) {
        std::string *out = static_cast<std::string *>(png_get_io_ptr(png_ptr_));
        out->append(reinterpret_cast<char *>(data), length);
    }, nullptr);

    struct ptrs {
        ptrs(size_t count) : rows(new png_bytep[count]) {}
        ~ptrs() { delete[] rows; }
        png_bytep *rows = nullptr;
    } pointers(src.size.height);

    for (size_t i = 0; i < src.size.height; i++) {
        pointers.rows[i] = src.data.get() + src.stride() * i;
    }

    png_set_rows(png_ptr, info_ptr, pointers.rows);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return result;
}

#if !defined(__ANDROID__) && !defined(__APPLE__)
PremultipliedImage decodeWebP(const uint8_t*, size_t);
#endif // !defined(__ANDROID__) && !defined(__APPLE__)

PremultipliedImage decodePNG(const uint8_t*, size_t);
PremultipliedImage decodeJPEG(const uint8_t*, size_t);

PremultipliedImage decodeImage(const std::string& string) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(string.data());
    const size_t size = string.size();

#if !defined(__ANDROID__) && !defined(__APPLE__)
    if (size >= 12) {
        uint32_t riff_magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        uint32_t webp_magic = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
        if (riff_magic == 0x52494646 && webp_magic == 0x57454250) {
            return decodeWebP(data, size);
        }
    }
#endif // !defined(__ANDROID__) && !defined(__APPLE__)

    if (size >= 4) {
        uint32_t magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        if (magic == 0x89504E47U) {
            return decodePNG(data, size);
        }
    }

    if (size >= 2) {
        uint16_t magic = ((data[0] << 8) | data[1]) & 0xffff;
        if (magic == 0xFFD8) {
            return decodeJPEG(data, size);
        }
    }

    throw std::runtime_error("unsupported image type");
}

} // namespace mbgl
