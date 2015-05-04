#include <mbgl/util/work_request.hpp>

namespace mbgl {

WorkRequest::WorkRequest() = default;

WorkRequest::WorkRequest(Future&& future)
    : complete(std::move(future)) {
}

WorkRequest::WorkRequest(WorkRequest&& o)
    : complete(std::move(o.complete)) {
}

WorkRequest::~WorkRequest() {
    if (complete.valid()) {
        complete.get();
    }
}

WorkRequest& WorkRequest::operator=(WorkRequest&& o) {
    complete = std::move(o.complete);
    return *this;
}

WorkRequest::operator bool() const {
    return complete.valid();
}

void WorkRequest::join() {
    complete.get();
}

}
