#include "mock_file_source.hpp"
#include <mbgl/util/io.hpp>

namespace mbgl {

class MockFileRequest : public FileRequest {
public:
    MockFileRequest(MockFileSource& fileSource_)
        : fileSource(fileSource_) {
    }

    ~MockFileRequest() {
        fileSource.pending.erase(this);
    }

    MockFileSource& fileSource;
};

MockFileSource::MockFileSource(Type type_, const std::string& match_)
    : type(type_), match(match_) {
    timer.start(std::chrono::milliseconds(10), std::chrono::milliseconds(10), [this] {
        // Explicit move to avoid iterator invalidation if ~MockFileRequest gets called within the loop.
        auto pending_ = std::move(pending);
        for (auto& pair : pending_) {
            respond(pair.second.first, pair.second.second);
        }
    });
}

MockFileSource::~MockFileSource() {
    timer.stop();
}

void MockFileSource::respond(Resource resource, Callback callback) const {
    if (type == Type::Success || resource.url.find(match) == std::string::npos) {
        Response res;
        try {
            res.data = std::make_shared<const std::string>(util::read_file(resource.url));
        } catch (const std::exception& err) {
            res.error = std::make_unique<Response::Error>(Response::Error::Reason::Other, err.what());
        }
        callback(res);
    } else if (type == Type::RequestFail) {
        Response res;
        res.error = std::make_unique<Response::Error>(Response::Error::Reason::Other, "Failed by the test case");
        callback(res);
    } else if (type == Type::RequestWithCorruptedData) {
        Response res;
        auto data = std::make_shared<std::string>(util::read_file(resource.url));
        data->insert(0, "CORRUPTED");
        res.data = std::move(data);
        callback(res);
    }
}

std::unique_ptr<FileRequest> MockFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<MockFileRequest>(*this);

    pending.emplace(req.get(), std::make_pair(resource, callback));

    if (requestEnqueuedCallback && resource.url.find(match) != std::string::npos) {
        requestEnqueuedCallback();
    }

    return std::move(req);
}

} // namespace mbgl
