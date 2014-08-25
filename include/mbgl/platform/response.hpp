#ifndef MBGL_PLATFORM_RESPONSE
#define MBGL_PLATFORM_RESPONSE

#include <string>
#include <functional>
#include <ctime>

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace platform {

struct Response : private util::noncopyable {
    Response(std::function<void(Response *)> callback) : callback(callback) {}
    int16_t code = -1;
    std::string body;
    std::string error_message;
    std::time_t modified;
    std::time_t expires;
    std::function<void(Response *)> callback;

    void setCacheControl(const char *value);
    void setLastModified(const char *value);
};

}
}

#endif
