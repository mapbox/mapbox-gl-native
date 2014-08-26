#include <mbgl/platform/response.hpp>
#include <curl/curl.h>

#include <cstdio>

namespace mbgl {
namespace platform {


void Response::setCacheControl(const char *value) {
    if (!value) {
        expires = -1;
        return;
    }

    int seconds = 0;
    // TODO: cache-control may contain other information as well:
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.9
    if (std::sscanf(value, "max-age=%u", &seconds) == 1) {
        if (std::time(&expires) != -1) {
            expires += seconds;
        }
    }
}

void Response::setLastModified(const char *value) {
    modified = curl_getdate(value, nullptr);
}

}
}
