#include "mbgl/util/image_reader.hpp"
//#include <mapnik/color.hpp>

// jpeg
extern "C"
{
#include <jpeglib.h>
}

// boost
#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#pragma GCC diagnostic pop

// std
#include <cstdio>
#include <memory>

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
    explicit jpeg_reader(std::string const& file_name);
    explicit jpeg_reader(char const* data, size_t size);
    ~jpeg_reader();
    unsigned width() const;
    unsigned height() const;
    inline bool has_alpha() const { return false; }
    inline bool premultiplied_alpha() const { return true; }
    void read(unsigned x,unsigned y, unsigned width, unsigned height, char *image);
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

namespace
{
image_reader* create_jpeg_reader(std::string const& file)
{
    return new jpeg_reader<boost::iostreams::file_source>(file);
}

image_reader* create_jpeg_reader2(char const* data, size_t size)
{
    return new jpeg_reader<boost::iostreams::array_source>(data, size);
}

const bool registered  = register_image_reader("jpeg",create_jpeg_reader);
const bool registered2 = register_image_reader("jpeg",create_jpeg_reader2);
}

// ctors
template <typename T>
jpeg_reader<T>::jpeg_reader(std::string const& file_name)
    : source_(file_name,std::ios_base::in | std::ios_base::binary),
      stream_(source_),
      width_(0),
      height_(0)
{
    if (!stream_) throw image_reader_exception("cannot open image file "+ file_name);
    init();
}

template <typename T>
jpeg_reader<T>::jpeg_reader(char const* data, size_t size)
    : source_(data, size),
      stream_(source_),
      width_(0),
      height_(0)
{
    if (!stream_) throw image_reader_exception("cannot open image stream");
    init();
}

// dtor
template <typename T>
jpeg_reader<T>::~jpeg_reader() {}

// jpeg stream wrapper
template <typename T>
void jpeg_reader<T>::init_source (j_decompress_ptr cinfo)
{
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);
    wrap->stream->seekg(0,std::ios_base::beg);
}

template <typename T>
boolean jpeg_reader<T>::fill_input_buffer (j_decompress_ptr cinfo)
{
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);
    wrap->stream->read(reinterpret_cast<char*>(&wrap->buffer[0]),BUF_SIZE);
    std::streamsize size = wrap->stream->gcount();
    wrap->manager.next_input_byte = wrap->buffer;
    wrap->manager.bytes_in_buffer = BUF_SIZE;
    return (size > 0) ? TRUE : FALSE;
}

template <typename T>
void jpeg_reader<T>::skip(j_decompress_ptr cinfo, long count)
{
    if (count <= 0) return; //A zero or negative skip count should be treated as a no-op.
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);

    if (wrap->manager.bytes_in_buffer > 0 && count < static_cast<long>(wrap->manager.bytes_in_buffer))
    {
        wrap->manager.bytes_in_buffer -= count;
        wrap->manager.next_input_byte = &wrap->buffer[BUF_SIZE - wrap->manager.bytes_in_buffer];
    }
    else
    {
        wrap->stream->seekg(count - wrap->manager.bytes_in_buffer, std::ios_base::cur);
        // trigger buffer fill
        wrap->manager.next_input_byte = 0;
        wrap->manager.bytes_in_buffer = 0; //bytes_in_buffer may be zero on return.
    }
}

template <typename T>
void jpeg_reader<T>::term (j_decompress_ptr /*cinfo*/)
{
// no-op
}

template <typename T>
void jpeg_reader<T>::attach_stream (j_decompress_ptr cinfo, input_stream* in)
{
    if (cinfo->src == 0)
    {
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(jpeg_stream_wrapper));
    }
    jpeg_reader::jpeg_stream_wrapper * src = reinterpret_cast<jpeg_reader::jpeg_stream_wrapper*> (cinfo->src);
    src->manager.init_source = init_source;
    src->manager.fill_input_buffer = fill_input_buffer;
    src->manager.skip_input_data = skip;
    src->manager.resync_to_restart = jpeg_resync_to_restart;
    src->manager.term_source = term;
    src->manager.bytes_in_buffer = 0;
    src->manager.next_input_byte = 0;
    src->stream = in;
}

template <typename T>
void jpeg_reader<T>::on_error(j_common_ptr /*cinfo*/)
{
}

template <typename T>
void jpeg_reader<T>::on_error_message(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    throw image_reader_exception(std::string("JPEG Reader: libjpeg could not read image: ") + buffer);
}

template <typename T>
void jpeg_reader<T>::init()
{
    jpeg_decompress_struct cinfo;
    jpeg_info_guard iguard(&cinfo);
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = on_error;
    jerr.output_message = on_error_message;
    jpeg_create_decompress(&cinfo);
    attach_stream(&cinfo, &stream_);
    int ret = jpeg_read_header(&cinfo, TRUE);
    if (ret != JPEG_HEADER_OK)
        throw image_reader_exception("JPEG Reader: failed to read header");
    jpeg_start_decompress(&cinfo);
    width_ = cinfo.output_width;
    height_ = cinfo.output_height;

    if (cinfo.out_color_space == JCS_UNKNOWN)
    {
        throw image_reader_exception("JPEG Reader: failed to read unknown color space");
    }
    if (cinfo.output_width == 0 || cinfo.output_height == 0)
    {
        throw image_reader_exception("JPEG Reader: failed to read image size of");
    }
}

template <typename T>
unsigned jpeg_reader<T>::width() const
{
    return width_;
}

template <typename T>
unsigned jpeg_reader<T>::height() const
{
    return height_;
}

template <typename T>
void jpeg_reader<T>::read(unsigned x0, unsigned y0, unsigned width, unsigned height, char* image)
{
    stream_.clear();
    stream_.seekg(0, std::ios_base::beg);

    jpeg_decompress_struct cinfo;
    jpeg_info_guard iguard(&cinfo);
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = on_error;
    jerr.output_message = on_error_message;
    jpeg_create_decompress(&cinfo);
    attach_stream(&cinfo, &stream_);
    int ret = jpeg_read_header(&cinfo, TRUE);
    if (ret != JPEG_HEADER_OK) throw image_reader_exception("JPEG Reader read(): failed to read header");
    jpeg_start_decompress(&cinfo);
    JSAMPARRAY buffer;
    int row_stride;
    unsigned char r,g,b;
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    unsigned w = std::min(width,width_ - x0);
    unsigned h = std::min(height,height_ - y0);

    const std::unique_ptr<unsigned int[]> out_row(new unsigned int[w]);
    unsigned row = 0;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        if (row >= y0 && row < y0 + h)
        {
            for (unsigned int x = 0; x < w; ++x)
            {
                unsigned col = x + x0;
                r = buffer[0][cinfo.output_components * col];
                if (cinfo.output_components > 2)
                {
                    g = buffer[0][cinfo.output_components * col + 1];
                    b = buffer[0][cinfo.output_components * col + 2];
                } else {
                    g = r;
                    b = r;
                }
                out_row[x] =  (0xff << 24) | (b << 16) | (g << 8) | r;//color(r, g, b, a).rgba();
            }
            //image.setRow(row - y0, out_row.get(), w);
            std::copy((char*)out_row.get(), (char*)out_row.get() + w*4, image + (row - y0)*width_*4);
        }
        ++row;
    }
    jpeg_finish_decompress(&cinfo);
}

}}
