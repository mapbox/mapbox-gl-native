#include "mbgl/util/image_reader.hpp"
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
    explicit png_reader(std::string const& file_name);
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

namespace
{

image_reader* create_png_reader(std::string const& file)
{
      return new png_reader<boost::iostreams::file_source>(file);
}

image_reader* create_png_reader2(char const * data, std::size_t size)
{
    return new png_reader<boost::iostreams::array_source>(data, size);
}

const static bool registered = register_image_reader("png",create_png_reader);
const static bool registered2 = register_image_reader("png", create_png_reader2);
}


void user_error_fn(png_structp /*png_ptr*/, png_const_charp error_msg)
{
    throw image_reader_exception(std::string("failed to read invalid png: '") + error_msg + "'");
}

void user_warning_fn(png_structp /*png_ptr*/, png_const_charp warning_msg)
{
    fprintf(stderr, "ImageReader (PNG): %s\n", warning_msg);

}

template <typename T>
void png_reader<T>::png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
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
png_reader<T>::png_reader(std::string const& file_name)
    : source_(file_name,std::ios_base::in | std::ios_base::binary),
      stream_(source_),
      width_(0),
      height_(0),
      bit_depth_(0),
      color_type_(0),
      has_alpha_(false)
{
    if (!source_.is_open()) throw image_reader_exception("PNG reader: cannot open file '"+ file_name + "'");
    if (!stream_) throw image_reader_exception("PNG reader: cannot open file '"+ file_name + "'");
    init();
}

template <typename T>
png_reader<T>::png_reader(char const* data, std::size_t size)
    : source_(data,size),
      stream_(source_),
      width_(0),
      height_(0),
      bit_depth_(0),
      color_type_(0),
      has_alpha_(false)
{

    if (!stream_) throw image_reader_exception("PNG reader: cannot open image stream");
    init();
}


template <typename T>
png_reader<T>::~png_reader() {}


template <typename T>
void png_reader<T>::init()
{
    png_byte header[8];
    std::memset(header,0,8);
    stream_.read(reinterpret_cast<char*>(header),8);
    if ( stream_.gcount() != 8)
    {
        throw image_reader_exception("PNG reader: Could not read image");
    }
    int is_png=!png_sig_cmp(header,0,8);
    if (!is_png)
    {
        throw image_reader_exception("File or stream is not a png");
    }
    png_structp png_ptr = png_create_read_struct
        (PNG_LIBPNG_VER_STRING,0,0,0);

    if (!png_ptr)
    {
        throw image_reader_exception("failed to allocate png_ptr");
    }

    // catch errors in a custom way to avoid the need for setjmp
    png_set_error_fn(png_ptr, png_get_error_ptr(png_ptr), user_error_fn, user_warning_fn);

    png_infop info_ptr;
    png_struct_guard sguard(&png_ptr,&info_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) throw image_reader_exception("failed to create info_ptr");

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
unsigned png_reader<T>::width() const
{
    return width_;
}

template <typename T>
unsigned png_reader<T>::height() const
{
    return height_;
}

template <typename T>
void png_reader<T>::read(unsigned x0, unsigned y0, unsigned w, unsigned h, char * image)
{
    stream_.clear();
    stream_.seekg(0, std::ios_base::beg);

    png_structp png_ptr = png_create_read_struct
        (PNG_LIBPNG_VER_STRING,0,0,0);

    if (!png_ptr)
    {
        throw image_reader_exception("failed to allocate png_ptr");
    }

    // catch errors in a custom way to avoid the need for setjmp
    png_set_error_fn(png_ptr, png_get_error_ptr(png_ptr), user_error_fn, user_warning_fn);

    png_infop info_ptr;
    png_struct_guard sguard(&png_ptr,&info_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) throw image_reader_exception("failed to create info_ptr");

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
            rows[row] = (png_bytep)image + row * width_ ;
        png_read_image(png_ptr, rows.get());
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
                std::copy(&row[x0 * 4], &row[x0 * 4] + w, image + i * width_* 4);
            }
        }
    }
    png_read_end(png_ptr,0);
}
}}
