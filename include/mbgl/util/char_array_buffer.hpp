#pragma once

#include <streambuf>

namespace mbgl {
namespace util {

// ref https://artofcode.wordpress.com/2010/12/12/deriving-from-stdstreambuf/

class CharArrayBuffer : public std::streambuf
{
public:
    CharArrayBuffer(char const* data, std::size_t size)
        : begin_(data), end_(data + size), current_(data) {}

private:
    int_type underflow() final {
        if (current_ == end_) {
            return traits_type::eof();
        }
        return traits_type::to_int_type(*current_);
    }

    int_type uflow() final {
        if (current_ == end_) {
            return traits_type::eof();
        }
        return traits_type::to_int_type(*current_++);
    }

    int_type pbackfail(int_type ch) final {
        if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1])) {
            return traits_type::eof();
        }
        return traits_type::to_int_type(*--current_);
    }

    std::streamsize showmanyc() final {
        return end_ - current_;
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) final {
        if (dir == std::ios_base::beg) current_ = std::min(begin_ + off, end_);
        else if (dir == std::ios_base::cur) current_ = std::min(current_ + off, end_);
        else current_ = std::max(end_ - off, begin_); // dir == std::ios_base::end
        return pos_type(off_type(current_ - begin_));
    }

    char const * const begin_;
    char const * const end_;
    char const * current_;
};

} // namespace util
} // namespace mbgl
