#ifndef MBGL_STORAGE_RESPONSE
#define MBGL_STORAGE_RESPONSE

#include <string>
#include <memory>

namespace mbgl {

class Response {
public:
    enum Status : bool { Error, Successful };

    Status status = Error;
    std::shared_ptr<const std::string> data;
    std::string message;
    int64_t modified = 0;
    int64_t expires = 0;
    std::string etag;
};

}

#endif
