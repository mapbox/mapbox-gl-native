#ifndef LLMR_LINUX_REQUEST
#define LLMR_LINUX_REQUEST

#include <string>
#include <functional>

typedef void CURL;

namespace llmr {
namespace platform {

typedef struct curl_context_s curl_context_t;
struct Response;

class Request {
public:
    Request(std::string url, std::function<void(Response *)> fn);

public:
    const std::string url;
    const std::function<void(Response *)> fn;
    Response *res = nullptr;

private:
    CURL *curl = nullptr;
};
}
}

#endif
