#ifndef TEST_RESOURCES_STUB_FILE_SOURCE
#define TEST_RESOURCES_STUB_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/timer.hpp>

#include <unordered_map>

namespace mbgl {

class StubFileSource : public FileSource {
public:
    StubFileSource();
    ~StubFileSource() override;

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

    // You can set the response callback on a global level by assigning this callback:
    std::function<Response (const Resource&)> response = [this] (const Resource& resource) {
        return defaultResponse(resource);
    };

    // Or set per-kind responses by setting these callbacks:
    std::function<Response (const Resource&)> styleResponse;
    std::function<Response (const Resource&)> sourceResponse;
    std::function<Response (const Resource&)> tileResponse;
    std::function<Response (const Resource&)> glyphsResponse;
    std::function<Response (const Resource&)> spriteJSONResponse;
    std::function<Response (const Resource&)> spriteImageResponse;

private:
    friend class StubFileRequest;

    // The default behavior is to throw if no per-kind callback has been set.
    Response defaultResponse(const Resource&);

    std::unordered_map<FileRequest*, std::pair<Response, Callback>> pending;
    util::Timer timer;
};

}

#endif
