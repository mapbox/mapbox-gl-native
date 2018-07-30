#include <mbgl/util/platform.hpp>
#include <libnu/casemap.h>
#include <cstring>
#include <sstream>

namespace mbgl { namespace platform {

std::string uppercase(const std::string& str)
{
    std::stringstream output;
    char const *itr = str.c_str(), *nitr;
    char const *end = itr + str.length();
    char lo[5] = { 0 };

    for (; itr < end; itr = nitr)
    {
        uint32_t code_point = 0;
        char const* buf = nullptr;

        nitr = _nu_toupper(itr, end, nu_utf8_read, &code_point, &buf, nullptr);
        if (buf != nullptr)
        {
            do
            {
                buf = NU_CASEMAP_DECODING_FUNCTION(buf, &code_point);
                if (code_point == 0) break;
                output.write(lo, nu_utf8_write(code_point, lo) - lo);
            }
            while (code_point != 0);
        }
        else
        {
            output.write(itr, nitr - itr);
        }
    }

    return output.str();

}

std::string lowercase(const std::string& str)
{
    std::stringstream output;
    char const *itr = str.c_str(), *nitr;
    char const *end = itr + str.length();
    char lo[5] = { 0 };

    for (; itr < end; itr = nitr)
    {
        uint32_t code_point = 0;
        char const* buf = nullptr;

        nitr = _nu_tolower(itr, end, nu_utf8_read, &code_point, &buf, nullptr);
        if (buf != nullptr)
        {
            do
            {
                buf = NU_CASEMAP_DECODING_FUNCTION(buf, &code_point);
                if (code_point == 0) break;
                output.write(lo, nu_utf8_write(code_point, lo) - lo);
            }
            while (code_point != 0);
        }
        else
        {
            output.write(itr, nitr - itr);
        }
    }

    return output.str();
}

} // namespace platform
} // namespace mbgl
