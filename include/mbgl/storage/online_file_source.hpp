#ifndef MBGL_STORAGE_ONLINE_FILE_SOURCE
#define MBGL_STORAGE_ONLINE_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class OnlineFileSource : public FileSource {
public:
    OnlineFileSource();
    ~OnlineFileSource() override;

    void setAccessToken(const std::string& t) { accessToken = t; }
    std::string getAccessToken() const { return accessToken; }

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

private:
    friend class OnlineFileRequestImpl;

    class Impl;
    const std::unique_ptr<util::Thread<Impl>> thread;
    std::string accessToken;
};

} // namespace mbgl

#endif
