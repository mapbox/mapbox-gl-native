#include <mbgl/util/image.hpp>

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
#include <jpeglib.h>
}

namespace mbgl {

using source_type = boost::iostreams::array_source;
using input_stream = boost::iostreams::stream<source_type>;

const static unsigned BUF_SIZE = 4096;

struct jpeg_stream_wrapper {
    jpeg_source_mgr manager;
    input_stream * stream;
    JOCTET buffer[BUF_SIZE];
};

static void init_source(j_decompress_ptr cinfo) {
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);
    wrap->stream->seekg(0,std::ios_base::beg);
}

static boolean fill_input_buffer(j_decompress_ptr cinfo) {
    jpeg_stream_wrapper* wrap = reinterpret_cast<jpeg_stream_wrapper*>(cinfo->src);
    wrap->stream->read(reinterpret_cast<char*>(&wrap->buffer[0]),BUF_SIZE);
    std::streamsize size = wrap->stream->gcount();
    wrap->manager.next_input_byte = wrap->buffer;
    wrap->manager.bytes_in_buffer = BUF_SIZE;
    return (size > 0) ? TRUE : FALSE;
}

static void skip(j_decompress_ptr cinfo, long count) {
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

static void term(j_decompress_ptr) {}

static void attach_stream(j_decompress_ptr cinfo, input_stream* in) {
    if (cinfo->src == 0) {
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(jpeg_stream_wrapper));
    }
    jpeg_stream_wrapper * src = reinterpret_cast<jpeg_stream_wrapper*> (cinfo->src);
    src->manager.init_source = init_source;
    src->manager.fill_input_buffer = fill_input_buffer;
    src->manager.skip_input_data = skip;
    src->manager.resync_to_restart = jpeg_resync_to_restart;
    src->manager.term_source = term;
    src->manager.bytes_in_buffer = 0;
    src->manager.next_input_byte = 0;
    src->stream = in;
}

static void on_error(j_common_ptr) {}

static void on_error_message(j_common_ptr cinfo) {
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    throw std::runtime_error(std::string("JPEG Reader: libjpeg could not read image: ") + buffer);
}

struct jpeg_info_guard {
    jpeg_info_guard(jpeg_decompress_struct* cinfo)
        : i_(cinfo) {}

    ~jpeg_info_guard() {
        jpeg_destroy_decompress(i_);
    }

    jpeg_decompress_struct* i_;
};

PremultipliedImage decodeJPEG(const uint8_t* data, size_t size) {
    source_type source(reinterpret_cast<const char*>(data), size);
    input_stream stream(source);

    jpeg_decompress_struct cinfo;
    jpeg_info_guard iguard(&cinfo);
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = on_error;
    jerr.output_message = on_error_message;
    jpeg_create_decompress(&cinfo);
    attach_stream(&cinfo, &stream);

    int ret = jpeg_read_header(&cinfo, TRUE);
    if (ret != JPEG_HEADER_OK)
        throw std::runtime_error("JPEG Reader: failed to read header");

    jpeg_start_decompress(&cinfo);

    if (cinfo.out_color_space == JCS_UNKNOWN)
        throw std::runtime_error("JPEG Reader: failed to read unknown color space");

    if (cinfo.output_width == 0 || cinfo.output_height == 0)
        throw std::runtime_error("JPEG Reader: failed to read image size");

    size_t width = cinfo.output_width;
    size_t height = cinfo.output_height;
    size_t components = cinfo.output_components;
    size_t rowStride = components * width;

    PremultipliedImage image { width, height };
    uint8_t* dst = image.data.get();

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, rowStride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);

        for (size_t i = 0; i < width; ++i) {
            dst[0] = buffer[0][components * i];
            dst[3] = 0xFF;

            if (components > 2) {
                dst[1] = buffer[0][components * i + 1];
                dst[2] = buffer[0][components * i + 2];
            } else {
                dst[1] = dst[0];
                dst[2] = dst[0];
            }

            dst += 4;
        }
    }

    jpeg_finish_decompress(&cinfo);

    return std::move(image);
}

}
