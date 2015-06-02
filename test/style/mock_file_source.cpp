#include "../fixtures/util.hpp"
#include "mock_file_source.hpp"

#include <mbgl/storage/request.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/thread.hpp>

namespace mbgl {

class MockFileSource::Impl {
public:
    Impl(uv_loop_t*, Type type, const std::string& match) : type_(type), match_(match) {}

    void handleRequest(Request* req) const;

private:
    void replyWithFailure(Response* res) const;
    void replyWithCorruptedData(Response* res, const std::string& url) const;
    void replyWithSuccess(Response* res, const std::string& url) const;

    Type type_;
    std::string match_;
};

void MockFileSource::Impl::replyWithFailure(Response* res) const {
    res->status = Response::Status::Error;
    res->message = "Failed by the test case";
}

void MockFileSource::Impl::replyWithCorruptedData(Response* res, const std::string& url) const {
    res->status = Response::Status::Successful;
    res->data = util::read_file(url);
    res->data.insert(0, "CORRUPTED");
}

void MockFileSource::Impl::replyWithSuccess(Response* res, const std::string& url) const {
    res->status = Response::Status::Successful;
    res->data = util::read_file(url);
}

void MockFileSource::Impl::handleRequest(Request* req) const {
    const std::string& url = req->resource.url;
    std::shared_ptr<Response> response = std::make_shared<Response>();

    if (url.find(match_) == std::string::npos) {
        replyWithSuccess(response.get(), url);
        req->notify(response);
        return;
    }

    switch (type_) {
    case Type::Success:
        replyWithSuccess(response.get(), url);
        break;
    case Type::RequestFail:
        replyWithFailure(response.get());
        break;
    case Type::RequestWithCorruptedData:
        replyWithCorruptedData(response.get(), url);
        break;
    default:
        EXPECT_TRUE(false) << "Should never be reached.";
    }

    req->notify(response);
}

MockFileSource::MockFileSource(Type type, const std::string& match)
    : thread_(std::make_unique<util::Thread<Impl>>("FileSource", util::ThreadPriority::Low, type, match)) {
}

Request* MockFileSource::request(const Resource& resource, uv_loop_t* loop, Callback callback) {
    Request* req = new Request(resource, loop, std::move(callback));
    thread_->invoke(&Impl::handleRequest, req);

    return req;
}

void MockFileSource::cancel(Request*) {
}

}
