#ifndef MBGL_UTIL_JPEG_READER_HPP
#define MBGL_UTIL_JPEG_READER_HPP

#include <mbgl/platform/default/image_reader.hpp>

// jpeg
extern "C"
{
#include <jpeglib.h>
}

#include <boost/iostreams/stream.hpp>

namespace mbgl { namespace util {

template <typename T>
class jpeg_reader : public image_reader
{
public:
    using source_type = T;
    using input_stream = boost::iostreams::stream<source_type>;
    const static unsigned BUF_SIZE = 4096;
private:
    struct jpeg_stream_wrapper
    {
        jpeg_source_mgr manager;
        input_stream * stream;
        JOCTET buffer[BUF_SIZE];
    };

    struct jpeg_info_guard
    {
        jpeg_info_guard(jpeg_decompress_struct * cinfo)
            : i_(cinfo) {}

        ~jpeg_info_guard()
        {
            jpeg_destroy_decompress(i_);
        }
        jpeg_decompress_struct * i_;
    };

private:
    source_type source_;
    input_stream stream_;
    unsigned width_;
    unsigned height_;
public:
    jpeg_reader(char const* data, size_t size);
    ~jpeg_reader();
    unsigned width() const;
    unsigned height() const;
    inline bool has_alpha() const { return false; }
    inline bool premultiplied_alpha() const { return true; }
    void read(unsigned x,unsigned y, unsigned w, unsigned h, char *image);
private:
    void init();
    static void on_error(j_common_ptr cinfo);
    static void on_error_message(j_common_ptr cinfo);
    static void init_source(j_decompress_ptr cinfo);
    static boolean fill_input_buffer(j_decompress_ptr cinfo);
    static void skip(j_decompress_ptr cinfo, long count);
    static void term(j_decompress_ptr cinfo);
    static void attach_stream(j_decompress_ptr cinfo, input_stream* in);
};

}}

#endif // MBGL_UTIL_JPEG_READER_HPP
