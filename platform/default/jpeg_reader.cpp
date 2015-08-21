#include <mbgl/platform/default/jpeg_reader.hpp>

// boost
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/array.hpp>


// std
#include <cstdio>
#include <memory>

namespace mbgl { namespace util {

// ctor
template <typename T>
JpegReader<T>::JpegReader(char const* data, size_t size)
    : source_(data, size),
      stream_(source_),
      width_(0),
      height_(0)
{
    if (!stream_) throw ImageReaderException("cannot open image stream");
    init();
}

// dtor
template <typename T>
JpegReader<T>::~JpegReader() {}

// jpeg stream wrapper
template <typename T>
void JpegReader<T>::init_source (j_decompress_ptr cinfo)
{
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);
    wrap->stream->seekg(0,std::ios_base::beg);
}

template <typename T>
boolean JpegReader<T>::fill_input_buffer (j_decompress_ptr cinfo)
{
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);
    wrap->stream->read(reinterpret_cast<char*>(&wrap->buffer[0]),BUF_SIZE);
    std::streamsize size = wrap->stream->gcount();
    wrap->manager.next_input_byte = wrap->buffer;
    wrap->manager.bytes_in_buffer = BUF_SIZE;
    return (size > 0) ? TRUE : FALSE;
}

template <typename T>
void JpegReader<T>::skip(j_decompress_ptr cinfo, long count)
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
void JpegReader<T>::term (j_decompress_ptr /*cinfo*/)
{
// no-op
}

template <typename T>
void JpegReader<T>::attach_stream (j_decompress_ptr cinfo, input_stream* in)
{
    if (cinfo->src == 0)
    {
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(jpeg_stream_wrapper));
    }
    JpegReader::jpeg_stream_wrapper * src = reinterpret_cast<JpegReader::jpeg_stream_wrapper*> (cinfo->src);
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
void JpegReader<T>::on_error(j_common_ptr /*cinfo*/)
{
}

template <typename T>
void JpegReader<T>::on_error_message(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    throw ImageReaderException(std::string("JPEG Reader: libjpeg could not read image: ") + buffer);
}

template <typename T>
void JpegReader<T>::init()
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
        throw ImageReaderException("JPEG Reader: failed to read header");
    jpeg_start_decompress(&cinfo);
    width_ = cinfo.output_width;
    height_ = cinfo.output_height;

    if (cinfo.out_color_space == JCS_UNKNOWN)
    {
        throw ImageReaderException("JPEG Reader: failed to read unknown color space");
    }
    if (cinfo.output_width == 0 || cinfo.output_height == 0)
    {
        throw ImageReaderException("JPEG Reader: failed to read image size of");
    }
}

template <typename T>
unsigned JpegReader<T>::width() const
{
    return width_;
}

template <typename T>
unsigned JpegReader<T>::height() const
{
    return height_;
}

template <typename T>
void JpegReader<T>::read(unsigned x0, unsigned y0, unsigned w, unsigned h, char* image)
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
    if (ret != JPEG_HEADER_OK) throw ImageReaderException("JPEG Reader read(): failed to read header");
    jpeg_start_decompress(&cinfo);
    JSAMPARRAY buffer;
    int row_stride;
    unsigned char r,g,b;
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    w = std::min(w,width_ - x0);
    h = std::min(h,height_ - y0);

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
                out_row[x] =  (0xff << 24) | (b << 16) | (g << 8) | r;
            }
            std::copy((char*)out_row.get(), (char*)out_row.get() + w*4, image + (row - y0)*width_*4);
        }
        ++row;
    }
    jpeg_finish_decompress(&cinfo);
}

template class JpegReader<boost::iostreams::array_source>;

}}
