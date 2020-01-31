#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/timer.hpp>

#include <unordered_map>

namespace mbgl {

class StubFileSource : public FileSource {
public:
    enum class ResponseType {
        Asynchronous = 0,
        Synchronous
    };

    StubFileSource(ResponseType = ResponseType::Asynchronous);
    ~StubFileSource() override;

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;
    void remove(AsyncRequest*);

    using ResponseFunction = std::function<optional<Response> (const Resource&)>;

    // You can set the response callback on a global level by assigning this callback:
    ResponseFunction response = [this] (const Resource& resource) {
        return defaultResponse(resource);
    };

    // Or set per-kind responses by setting these callbacks:
    ResponseFunction styleResponse;
    ResponseFunction sourceResponse;
    ResponseFunction tileResponse;
    ResponseFunction glyphsResponse;
    ResponseFunction spriteJSONResponse;
    ResponseFunction spriteImageResponse;
    ResponseFunction imageResponse;

private:
    friend class StubOnlineFileSource;

    // The default behavior is to throw if no per-kind callback has been set.
    optional<Response> defaultResponse(const Resource&);

    std::unordered_map<AsyncRequest*, std::tuple<Resource, ResponseFunction, Callback>> pending;
    ResponseType type;
    util::Timer timer;
};

class StubOnlineFileSource : public StubFileSource, public OnlineFileSource {
public:

    StubOnlineFileSource(ResponseType t = ResponseType::Asynchronous) : StubFileSource(t) {};
    ~StubOnlineFileSource() override = default;

    std::unique_ptr<AsyncRequest> request(const Resource& r, Callback c) override { return StubFileSource::request(r, c); };
    void remove(AsyncRequest* r) { StubFileSource::remove(r); };
};


} // namespace mbgl
