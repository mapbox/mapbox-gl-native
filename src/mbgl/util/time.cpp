#include <mbgl/util/time.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <cstdio>

namespace mbgl {
namespace util {

static const char *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

std::string rfc1123(std::time_t time) {
    std::tm info;
    gmtime_r(&time, &info);
    char buffer[30];
    snprintf(buffer, 30, "%s, %02d %s %4d %02d:%02d:%02d GMT", week[info.tm_wday], info.tm_mday,
             months[info.tm_mon], 1900 + info.tm_year, info.tm_hour, info.tm_min, info.tm_sec);
    return buffer;
}

}
}
