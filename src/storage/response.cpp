#include <mbgl/storage/response.hpp>

#include <chrono>

namespace mbgl {

int64_t Response::parseCacheControl(const char *value) {
    if (value) {
        uint64_t seconds = 0;
        // TODO: cache-control may contain other information as well:
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.9
        if (std::sscanf(value, "max-age=%llu", static_cast<unsigned long long *>(&seconds)) == 1) {
            return std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count() +
                   seconds;
        }
    }

    return -1;
}

}
