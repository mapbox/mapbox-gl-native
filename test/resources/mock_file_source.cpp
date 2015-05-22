#include "mock_file_source.hpp"

#include <mbgl/storage/request.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/thread.hpp>

namespace mbgl {

class MockFileSource::Impl {
public:
    Impl(uv_loop_t*, const std::string& matchFail) : matchFail_(matchFail) {}

    void handleRequest(Request* req) const;

private:
    std::string matchFail_;
};

void MockFileSource::Impl::handleRequest(Request* req) const {
    const Resource& resource = req->resource;

    std::shared_ptr<Response> response = std::make_shared<Response>();
    if (matchFail_.empty() || resource.url.find(matchFail_) == std::string::npos) {
        response->status = Response::Status::Successful;
        response->data = util::read_file(resource.url.c_str());
    } else {
        response->message = "Failed by the test case";
    }

    req->notify(response);
}

MockFileSource::MockFileSource(const std::string& matchFail)
    : thread_(util::make_unique<util::Thread<Impl>>("FileSource", util::ThreadPriority::Low, matchFail)) {
}

Request* MockFileSource::request(const Resource& resource, uv_loop_t* loop, Callback callback) {
    Request* req = new Request(resource, loop, std::move(callback));
    thread_->invoke(&Impl::handleRequest, req);

    return req;
}

void MockFileSource::cancel(Request*) {
}

}
