#ifndef TEST_RESOURCES_MOCK_FILE_SOURCE
#define TEST_RESOURCES_MOCK_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/thread.hpp>

#include <string>
#include <memory>

namespace mbgl {

// The MockFileSource is a FileSource that can simulate different
// types of failures and it will work completely offline.
class MockFileSource : public FileSource {
public:
    // Success:
    //     Will reply to every request correctly with valid data.
    //
    // SuccessWithDelay:
    //     Will reply to every request correctly with valid data,
    //     but the ones that contains the "match" string on the
    //     URL will be answered after a delay. This can be useful
    //     for testing request cancellation.
    //
    // RequestFail:
    //     Will reply with an error to requests that contains
    //     the "match" string on the URL.
    //
    // RequestWithCorruptedData:
    //     Will answer every request successfully but will return
    //     corrupt data on the requests that contains the "match"
    //     string on the URL.
    enum Type {
        Success,
        SuccessWithDelay,
        RequestFail,
        RequestWithCorruptedData
    };

    class Impl;

    MockFileSource(Type type, const std::string& match);
    ~MockFileSource() override = default;

    // Function that gets called when a delayed resource is enqueued. The
    // callback must be safe to call from any thread.
    void setOnRequestDelayedCallback(std::function<void(void)> callback);

    // FileSource implementation.
    Request* request(const Resource&, uv_loop_t*, Callback) override;
    void cancel(Request*) override;

private:
    const std::unique_ptr<util::Thread<Impl>> thread_;
};

}

#endif
