#ifndef TEST_RESOURCES_MOCK_FILE_SOURCE
#define TEST_RESOURCES_MOCK_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/timer.hpp>

#include <string>
#include <unordered_map>

namespace mbgl {

// The MockFileSource is a FileSource that can simulate different
// types of failures and it will work completely offline.
class MockFileSource : public FileSource {
public:
    // Success:
    //     Will reply to every request correctly with valid data.
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
        RequestFail,
        RequestWithCorruptedData
    };

    MockFileSource(Type, const std::string& match);
    ~MockFileSource() override;

    // Function that gets called when a matching resource is enqueued.
    std::function<void (void)> requestEnqueuedCallback;

    // FileSource implementation.
    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

private:
    void respond(Resource, Callback) const;

    friend class MockFileRequest;

    Type type;
    std::string match;
    std::unordered_map<FileRequest*, std::pair<Resource, Callback>> pending;
    util::Timer timer;
};

}

#endif
