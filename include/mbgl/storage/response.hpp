#ifndef MBGL_STORAGE_RESPONSE
#define MBGL_STORAGE_RESPONSE

#include <string>
#include <ctime>

namespace mbgl {

class Response {
public:
    long code = 0;
    int64_t modified = 0;
    int64_t expires = 0;
    std::string data;

    std::string message;

    static int64_t parseCacheControl(const char *value);
};

}

#endif