#include "../fixtures/util.hpp"
#include "mock_file_source.hpp"

#include <mbgl/util/io.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/timer.hpp>

#include <algorithm>
#include <unordered_map>

namespace mbgl {

class MockFileRequest : public FileRequest {
public:
    MockFileRequest(const Resource& resource_,
                    MockFileSource& fileSource_)
        : resource(resource_),
          fileSource(fileSource_) {
    }

    ~MockFileRequest() {
        fileSource.cancel(this);
    }

    Resource resource;
    MockFileSource& fileSource;

    std::unique_ptr<WorkRequest> workRequest;
};

class MockFileSource::Impl {
public:
    Impl(Type type, const std::string& match)
        : type_(type), match_(match) {
        timer_.start(std::chrono::milliseconds(1000), std::chrono::milliseconds(1000), [this] { dispatchPendingRequests(); });
        timer_.unref();
    }

    ~Impl() {
        timer_.stop();
    }

    void setOnRequestDelayedCallback(std::function<void(void)> callback) {
        requestEnqueuedCallback_ = callback;
    }

    void handleRequest(FileRequest*, Resource, Callback);
    void cancelRequest(FileRequest*);

private:
    void replyWithSuccess(Resource, Callback) const;
    void replyWithSuccessWithDelay(FileRequest*, Resource, Callback);
    void replyWithFailure(Resource, Callback) const;
    void replyWithCorruptedData(Resource, Callback) const;

    void dispatchPendingRequests();

    Type type_;
    std::string match_;
    std::unordered_map<FileRequest*, std::pair<Resource, Callback>> pendingRequests_;
    util::Timer timer_;

    std::function<void(void)> requestEnqueuedCallback_;
};

void MockFileSource::Impl::replyWithSuccess(Resource resource, Callback callback) const {
    Response res;

    try {
        res.data = std::make_shared<const std::string>(std::move(util::read_file(resource.url)));
    } catch (const std::exception& err) {
        res.error = std::make_unique<Response::Error>(Response::Error::Reason::Other, err.what());
    }

    callback(res);
}

void MockFileSource::Impl::replyWithSuccessWithDelay(FileRequest* req, Resource resource, Callback callback) {
    if (resource.url.find(match_) == std::string::npos) {
        replyWithSuccess(resource, callback);
        return;
    }

    pendingRequests_.emplace(req, std::make_pair(resource, callback));
    requestEnqueuedCallback_();
}

void MockFileSource::Impl::replyWithFailure(Resource resource, Callback callback) const {
    if (resource.url.find(match_) == std::string::npos) {
        replyWithSuccess(resource, callback);
        return;
    }

    Response res;
    res.error = std::make_unique<Response::Error>(Response::Error::Reason::Other, "Failed by the test case");
    callback(res);
}

void MockFileSource::Impl::replyWithCorruptedData(Resource resource, Callback callback) const {
    if (resource.url.find(match_) == std::string::npos) {
        replyWithSuccess(resource, callback);
        return;
    }

    Response res;
    auto data = std::make_shared<std::string>(std::move(util::read_file(resource.url)));
    data->insert(0, "CORRUPTED");
    res.data = std::move(data);
    callback(res);
}

void MockFileSource::Impl::handleRequest(FileRequest* req, Resource resource, Callback callback) {
    switch (type_) {
    case Type::Success:
        replyWithSuccess(resource, callback);
        break;
    case Type::SuccessWithDelay:
        replyWithSuccessWithDelay(req, resource, callback);
        break;
    case Type::RequestFail:
        replyWithFailure(resource, callback);
        break;
    case Type::RequestWithCorruptedData:
        replyWithCorruptedData(resource, callback);
        break;
    default:
        EXPECT_TRUE(false) << "Should never be reached.";
    }
}

void MockFileSource::Impl::cancelRequest(FileRequest* req) {
    pendingRequests_.erase(req);
}

void MockFileSource::Impl::dispatchPendingRequests() {
    for (auto& pair : pendingRequests_) {
        replyWithSuccess(pair.second.first, pair.second.second);
    }

    pendingRequests_.clear();
}

MockFileSource::MockFileSource(Type type, const std::string& match)
    : thread_(std::make_unique<util::Thread<Impl>>(util::ThreadContext{"FileSource", util::ThreadType::Unknown, util::ThreadPriority::Low}, type, match)) {
}

void MockFileSource::setOnRequestDelayedCallback(std::function<void(void)> callback) {
    thread_->invokeSync(&Impl::setOnRequestDelayedCallback, callback);
}

std::unique_ptr<FileRequest> MockFileSource::request(const Resource& res, Callback callback) {
    auto req = std::make_unique<MockFileRequest>(res, *this);
    req->workRequest = thread_->invokeWithCallback(&Impl::handleRequest, callback, req.get(), res);
    return std::move(req);
}

void MockFileSource::cancel(FileRequest* req) {
    thread_->invoke(&Impl::cancelRequest, req);
}

}
