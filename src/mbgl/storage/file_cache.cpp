#include <mbgl/storage/file_cache.hpp>

namespace mbgl {

CacheRequest::CacheRequest() = default;

CacheRequest::CacheRequest(std::shared_ptr<bool> cancelled_)
    : cancelled(cancelled_) {
}

CacheRequest::CacheRequest(CacheRequest&& o)
    : cancelled(std::move(o.cancelled)) {
}

CacheRequest::~CacheRequest() {
    if (cancelled) {
        cancel();
    }
}

CacheRequest& CacheRequest::operator=(CacheRequest&& o) {
    cancelled = std::move(o.cancelled);
    return *this;
}

CacheRequest::operator bool() const {
    return cancelled.operator bool();
}

void CacheRequest::cancel() {
    *cancelled = true;
}

}
