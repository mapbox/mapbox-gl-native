#ifndef MBGL_STORAGE_RESPONSE
#define MBGL_STORAGE_RESPONSE

#include <string>

namespace mbgl {

class Response {
public:
    enum Status { Error, Successful, NotFound };

    Status status = Error;
    std::string message;
    int64_t modified = 0;
    int64_t expires = 0;
    std::string etag;
    std::string data;
};

}

#endif
