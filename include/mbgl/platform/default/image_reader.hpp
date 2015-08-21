#ifndef MBGL_UTIL_IMAGE_READER_HPP
#define MBGL_UTIL_IMAGE_READER_HPP

#include <mbgl/util/noncopyable.hpp>
// stl
#include <stdexcept>
#include <string>
#include <memory>

namespace mbgl { namespace util {

class ImageReaderException : public std::exception
{
private:
    std::string message_;
public:
    ImageReaderException(std::string const& message)
        : message_(message) {}

    ~ImageReaderException() throw() {}

    virtual const char* what() const throw()
    {
        return message_.c_str();
    }
};

struct ImageReader : private noncopyable
{
    virtual unsigned width() const=0;
    virtual unsigned height() const=0;
    virtual bool hasAlpha() const=0;
    virtual bool premultipliedAlpha() const=0;
    virtual void read(unsigned x,unsigned y, unsigned width, unsigned height, char* image)=0;
    virtual ~ImageReader() {}
};

std::unique_ptr<ImageReader> getImageReader(char const* data, size_t size);

}}

#endif
