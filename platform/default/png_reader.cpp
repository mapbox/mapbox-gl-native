#include <mbgl/platform/default/png_reader.hpp>
#include <mbgl/platform/log.hpp>
#include <iostream>
extern "C"
{
#include <png.h>
}

// boost
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

// stl
#include <cstring>
#include <memory>

namespace mbgl { namespace util {

void user_error_fn(png_structp /*png_ptr*/, png_const_charp error_msg)
{
    throw ImageReaderException(std::string("failed to read invalid png: '") + error_msg + "'");
}

void user_warning_fn(png_structp /*png_ptr*/, png_const_charp warning_msg)
{
    Log::Warning(Event::Image, "ImageReader (PNG): %s", warning_msg);
}

template <typename T>
void PngReader<T>::png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    input_stream * fin = reinterpret_cast<input_stream*>(png_get_io_ptr(png_ptr));
    fin->read(reinterpret_cast<char*>(data), length);
    std::streamsize read_count = fin->gcount();
    if (read_count < 0 || static_cast<png_size_t>(read_count) != length)
    {
        png_error(png_ptr, "Read Error");
    }
}

template <typename T>
PngReader<T>::PngReader(char const* data, std::size_t size)
    : source_(data,size),
      stream_(source_),
      width_(0),
      height_(0),
      bit_depth_(0),
      color_type_(0),
      has_alpha_(false)
{

    if (!stream_) throw ImageReaderException("PNG reader: cannot open image stream");
    init();
}


template <typename T>
PngReader<T>::~PngReader() {}


template <typename T>
void PngReader<T>::init()
{
    png_byte header[8];
    std::memset(header,0,8);
    stream_.read(reinterpret_cast<char*>(header),8);
    if ( stream_.gcount() != 8)
    {
        throw ImageReaderException("PNG reader: Could not read image");
    }
    int is_png=!png_sig_cmp(header,0,8);
    if (!is_png)
    {
        throw ImageReaderException("File or stream is not a png");
    }
    png_structp png_ptr = png_create_read_struct
        (PNG_LIBPNG_VER_STRING,0,0,0);

    if (!png_ptr)
    {
        throw ImageReaderException("failed to allocate png_ptr");
    }

    // catch errors in a custom way to avoid the need for setjmp
    png_set_error_fn(png_ptr, png_get_error_ptr(png_ptr), user_error_fn, user_warning_fn);

    png_infop info_ptr;
    png_struct_guard sguard(&png_ptr,&info_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) throw ImageReaderException("failed to create info_ptr");

    png_set_read_fn(png_ptr, (png_voidp)&stream_, png_read_data);

    png_set_sig_bytes(png_ptr,8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32  w, h;
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth_, &color_type_,0,0,0);
    has_alpha_ = (color_type_ & PNG_COLOR_MASK_ALPHA) || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
    width_=w;
    height_=h;
}

template <typename T>
unsigned PngReader<T>::width() const
{
    return width_;
}

template <typename T>
unsigned PngReader<T>::height() const
{
    return height_;
}

template <typename T>
void PngReader<T>::read(unsigned x0, unsigned y0, unsigned w, unsigned h, char * image)
{
    stream_.clear();
    stream_.seekg(0, std::ios_base::beg);

    png_structp png_ptr = png_create_read_struct
        (PNG_LIBPNG_VER_STRING,0,0,0);

    if (!png_ptr)
    {
        throw ImageReaderException("failed to allocate png_ptr");
    }

    // catch errors in a custom way to avoid the need for setjmp
    png_set_error_fn(png_ptr, png_get_error_ptr(png_ptr), user_error_fn, user_warning_fn);

    png_infop info_ptr;
    png_struct_guard sguard(&png_ptr,&info_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) throw ImageReaderException("failed to create info_ptr");

    png_set_read_fn(png_ptr, (png_voidp)&stream_, png_read_data);
    png_read_info(png_ptr, info_ptr);

    if (color_type_ == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type_ == PNG_COLOR_TYPE_GRAY && bit_depth_ < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth_ == 16)
        png_set_strip_16(png_ptr);
    if (color_type_ == PNG_COLOR_TYPE_GRAY ||
        color_type_ == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    // quick hack -- only work in >=libpng 1.2.7
    png_set_add_alpha(png_ptr,0xff,PNG_FILLER_AFTER); //rgba

    double gamma;
    if (png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, 2.2, gamma);

#ifdef PNG_ALPHA_PREMULTIPLIED
    png_set_alpha_mode(png_ptr, PNG_ALPHA_PREMULTIPLIED, PNG_GAMMA_LINEAR);
#endif

    if (x0 == 0 && y0 == 0 && w >= width_ && h >= height_)
    {
        if (png_get_interlace_type(png_ptr,info_ptr) == PNG_INTERLACE_ADAM7)
        {
            png_set_interlace_handling(png_ptr); // FIXME: libpng bug?
            // according to docs png_read_image
            // "..automatically handles interlacing,
            // so you don't need to call png_set_interlace_handling()"
        }
        png_read_update_info(png_ptr, info_ptr);
        // we can read whole image at once
        // alloc row pointers
        const std::unique_ptr<png_bytep[]> rows(new png_bytep[height_]);
        for (unsigned row = 0; row < height_; ++row)
            rows[row] = (png_bytep)image + row * width_ * 4 ;
        png_read_image(png_ptr, rows.get());
        fprintf(stderr, "manual premultiplication\n");

#ifndef PNG_ALPHA_PREMULTIPLIED
        // Manually premultiply the image if libpng didn't do it for us.
        for (unsigned row = 0; row < height_; ++row) {
            for (unsigned x = 0; x < width_; x++) {
                png_byte* ptr = &(rows[row][x * 4]);
                const float a = ptr[3] / 255.0f;
                ptr[0] *= a;
                ptr[1] *= a;
                ptr[2] *= a;
            }
        }
#endif
    }
    else
    {
        png_read_update_info(png_ptr, info_ptr);
        w=std::min(w, width_ - x0);
        h=std::min(h, height_ - y0);
        unsigned rowbytes=png_get_rowbytes(png_ptr, info_ptr);
        const std::unique_ptr<png_byte[]> row(new png_byte[rowbytes]);
        for (unsigned i = 0; i < height_; ++i)
        {
            png_read_row(png_ptr,row.get(),0);
            if (i >= y0 && i < (y0 + h))
            {
                // TODO: premultiply this
                std::copy(&row[x0 * 4], &row[x0 * 4] + w * 4, image + i * width_* 4);
            }
        }
    }
    png_read_end(png_ptr,0);
}

template class PngReader<boost::iostreams::array_source>;

}}
