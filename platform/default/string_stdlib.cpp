#include <mbgl/platform/platform.hpp>
#include <mbgl/util/utf.hpp>
#define NU_WITH_TOUPPER
#define NU_WITH_TOLOWER
#define NU_WITH_UTF8_WRITER
#include <libnu/libnu.h>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string& str)
{
    boost::u8_to_u32_iterator<std::string::const_iterator> itr(str.begin());
    boost::u8_to_u32_iterator<std::string::const_iterator> end(str.end());
    std::string output;
    char buf[4];
    for (; itr!=end; ++itr)
    {
        char const* up = nu_toupper(*itr);
        if (up != 0) output.append(up);
        else
        {
            std::memset(buf, 0, 4);
            nu_utf8_write(*itr, buf);
            output.append(buf);
        }
    }
    return output;
}

std::string lowercase(const std::string& str)
{
    boost::u8_to_u32_iterator<std::string::const_iterator> itr(str.begin());
    boost::u8_to_u32_iterator<std::string::const_iterator> end(str.end());
    std::string output;
    char buf[4];
    for (; itr!=end; ++itr)
    {
        char const* up = nu_tolower(*itr);
        if (up != 0) output.append(up);
        else
        {
            std::memset(buf, 0, 4);
            nu_utf8_write(*itr, buf);
            output.append(buf);
        }
    }
    return output;
}

}
}
