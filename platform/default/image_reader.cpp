#include <mbgl/platform/default/image_reader.hpp>
#include <mbgl/platform/default/png_reader.hpp>
#include <mbgl/platform/default/jpeg_reader.hpp>

#include <boost/optional.hpp>
#include <boost/iostreams/device/array.hpp>

namespace mbgl { namespace util {

inline boost::optional<std::string> type_from_bytes(char const* data, size_t size)
{
    using result_type = boost::optional<std::string>;
    if (size >= 4)
    {
        unsigned int magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        if (magic == 0x89504E47U)
        {
            return result_type("png");
        }
        else if (magic == 0x49492A00 || magic == 0x4D4D002A)
        {
            return result_type("tiff");
        }
    }
    if (size>=2)
    {
        unsigned int magic = ((data[0] << 8) | data[1]) & 0xffff;
        if (magic == 0xffd8)
        {
            return result_type("jpeg");
        }
    }

    if (size>=12)
    {
        if (data[0] == 'R' && data[1] == 'I' && data[2] == 'F' && data[3] == 'F' &&
            data[8] == 'W' && data[9] == 'E' && data[10] == 'B' && data[11] == 'P')
        {
            return result_type("webp");
        }
    }
    return result_type();
}

std::unique_ptr<ImageReader> getImageReader(char const* data, size_t size)
{
    boost::optional<std::string> type = type_from_bytes(data,size);
    if (type)
    {
        if (*type == "png")
        {
            return std::make_unique<PngReader<boost::iostreams::array_source>>(data, size);
        }
        else if (*type == "jpeg")
        {
            return std::make_unique<JpegReader<boost::iostreams::array_source>>(data, size);
        }
    }
    throw ImageReaderException("ImageReader: can't determine type from input data");
}

}}
