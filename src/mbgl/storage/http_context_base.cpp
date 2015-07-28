#include <mbgl/storage/http_context_base.hpp>

namespace mbgl {

HTTPContextBase::HTTPContextBase(uv_loop_t* loop_)
    : reachability(loop_, [this] { retryRequests(); }) {
    NetworkStatus::Subscribe(reachability.get());
    reachability.unref();
}

HTTPContextBase::~HTTPContextBase() {
    assert(requests.empty());
    NetworkStatus::Unsubscribe(reachability.get());
}

void HTTPContextBase::addRequest(HTTPRequestBase* request) {
    requests.insert(request);
}

void HTTPContextBase::removeRequest(HTTPRequestBase* request) {
    requests.erase(request);
}

void HTTPContextBase::retryRequests() {
    for (auto request : requests) {
        request->retry();
    }
}

}
