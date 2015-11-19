#ifndef MBGL_UTIL_HTTP_HEADER
#define MBGL_UTIL_HTTP_HEADER

#include <mapbox/optional.hpp>

#include <string>

namespace mbgl {
namespace http {

class CacheControl {
public:
    static CacheControl parse(const std::string&);

    mapbox::util::optional<uint64_t> maxAge;
    bool mustRevalidate = false;
};

} // namespace http
} // namespace mbgl

#endif
