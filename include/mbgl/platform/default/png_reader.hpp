#ifndef MBGL_UTIL_PNG_READER_HPP
#define MBGL_UTIL_PNG_READER_HPP

#include <mbgl/platform/default/image_reader.hpp>

extern "C"
{
#include <png.h>
}

#include <cstring>
#include <memory>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/iostreams/stream.hpp>
#pragma GCC diagnostic pop

namespace mbgl { namespace util {

template <typename T>
class PngReader : public ImageReader
{
    using source_type = T;
    using input_stream = boost::iostreams::stream<source_type>;

    struct png_struct_guard
    {
        png_struct_guard(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr)
            : p_(png_ptr_ptr),
              i_(info_ptr_ptr) {}

        ~png_struct_guard()
        {
            png_destroy_read_struct(p_,i_,0);
        }
        png_structpp p_;
        png_infopp i_;
    };

private:
    source_type source_;
    input_stream stream_;
    unsigned width_;
    unsigned height_;
    int bit_depth_;
    int color_type_;
    bool has_alpha_;
public:
    PngReader(char const* data, std::size_t size);
    ~PngReader();
    unsigned width() const;
    unsigned height() const;
    inline bool hasAlpha() const { return has_alpha_; }
    bool premultipliedAlpha() const { return true; } // png_set_alpha_mode(png, PNG_ALPHA_PREMULTIPLIED, 2.2)
    void read(unsigned x,unsigned y, unsigned width, unsigned height, char * image);
private:
    void init();
    static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
};


}}

#endif // MBGL_UTIL_PNG_READER_HPP
