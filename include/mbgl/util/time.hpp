#ifndef MBGL_UTIL_TIME
#define MBGL_UTIL_TIME

#include <mbgl/util/chrono.hpp>

#include <string>
#include <cstdint>
#include <ctime>

namespace mbgl {

namespace util {

// Returns the RFC1123 formatted date. E.g. "Tue, 04 Nov 2014 02:13:24 GMT"
std::string rfc1123(std::time_t time);

// YYYY-mm-dd HH:MM:SS e.g. "2015-11-26 16:11:23"
std::string iso8601(std::time_t time);

}

}

#endif
