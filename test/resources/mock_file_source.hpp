#ifndef TEST_RESOURCES_MOCK_FILE_SOURCE
#define TEST_RESOURCES_MOCK_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>

#include <string>

namespace mbgl {

namespace util {
template <typename T> class Thread;
}

// This mock FileSource will read data from the disk and will fail
// the request if the URL matches the 'matchFail' string.
class MockFileSource : public FileSource {
public:
    class Impl;

    MockFileSource(const std::string& matchFail);
    ~MockFileSource() override = default;

    // FileSource implementation.
    Request* request(const Resource&, uv_loop_t*, Callback) override;
    void cancel(Request*) override;

private:
    const std::unique_ptr<util::Thread<Impl>> thread_;
};

}

#endif
