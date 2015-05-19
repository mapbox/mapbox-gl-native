#include <mbgl/storage/http_context.hpp>

namespace mbgl {

HTTPContext::HTTPContext(uv_loop_t* loop_)
    : reachability(loop_, [this] { retryRequests(); }) {
    NetworkStatus::Subscribe(reachability.get());
    reachability.unref();
}

HTTPContext::~HTTPContext() {
    assert(requests.empty());
    NetworkStatus::Unsubscribe(reachability.get());
}

void HTTPContext::addRequest(RequestBase* request) {
    requests.insert(request);
}

void HTTPContext::removeRequest(RequestBase* request) {
    requests.erase(request);
}

void HTTPContext::retryRequests() {
    for (auto request : requests) {
        request->retry();
    }
}

}
