#ifndef MBGL_STORAGE_REQUEST_HOLDER
#define MBGL_STORAGE_REQUEST_HOLDER

#include <memory>

namespace mbgl {

class Request;

class RequestHolder {
public:
    inline RequestHolder& operator=(Request* req) {
        ptr = std::unique_ptr<Request, Deleter>(req);
        return *this;
    }

private:
    struct Deleter {
        void operator()(Request*) const;
    };
    std::unique_ptr<Request, Deleter> ptr;
};

}

#endif
