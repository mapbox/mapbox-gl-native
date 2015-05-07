#include <mbgl/util/work_request.hpp>

namespace mbgl {

WorkRequest::WorkRequest() = default;

WorkRequest::WorkRequest(Future&& future, JoinedFlag flag)
    : complete(std::move(future)),
      joinedFlag(flag) {
}

WorkRequest::WorkRequest(WorkRequest&& o)
    : complete(std::move(o.complete)),
      joinedFlag(std::move(o.joinedFlag)) {
}

WorkRequest::~WorkRequest() {
    if (complete.valid()) {
        join();
    }
}

WorkRequest& WorkRequest::operator=(WorkRequest&& o) {
    complete = std::move(o.complete);
    joinedFlag = std::move(o.joinedFlag);
    return *this;
}

WorkRequest::operator bool() const {
    return complete.valid();
}

void WorkRequest::join() {
    *joinedFlag = true;
    complete.get();
}

}
