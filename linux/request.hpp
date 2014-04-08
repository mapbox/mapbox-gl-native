#ifndef LLMR_LINUX_REQUEST
#define LLMR_LINUX_REQUEST

#include <string>
#include <functional>

#include <llmr/util/noncopyable.hpp>

// Forward definition.
typedef void CURL;

namespace llmr {
namespace platform {

struct Response;

class Request : private util::noncopyable {
public:
    Request(std::string url, std::function<void(Response *)> fn);

public:
    const std::string url;
    const std::function<void(Response *)> fn;
    std::unique_ptr<Response> res;
    CURL *curl = nullptr;
    bool cancelled = false;
};
}
}

#endif
