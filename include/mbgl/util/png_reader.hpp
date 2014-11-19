#ifndef MBGL_UTIL_PNG_READER_HPP
#define MBGL_UTIL_PNG_READER_HPP

#include "mbgl/util/image_reader.hpp"

extern "C"
{
#include <png.h>
}

#include <cstring>
#include <memory>

#include <boost/iostreams/stream.hpp>

namespace mbgl { namespace util {

template <typename T>
class png_reader : public image_reader
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
    png_reader(char const* data, std::size_t size);
    ~png_reader();
    unsigned width() const;
    unsigned height() const;
    inline bool has_alpha() const { return has_alpha_; }
    bool premultiplied_alpha() const { return false; } //http://www.libpng.org/pub/png/spec/1.1/PNG-Rationale.html
    void read(unsigned x,unsigned y, unsigned width, unsigned height, char * image);
private:
    void init();
    static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
};


}}

#endif // MBGL_UTIL_PNG_READER_HPP
