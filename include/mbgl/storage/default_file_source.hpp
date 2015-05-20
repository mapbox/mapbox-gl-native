#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/file_cache.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
}

class DefaultFileSource : public FileSource {
public:
    DefaultFileSource(FileCache *cache, const std::string &root = "");
    ~DefaultFileSource() override;

    void setAccessToken(const std::string& t) { accessToken = t; }
    std::string getAccessToken() const { return accessToken; }

    // FileSource API
    Request* request(const Resource&, uv_loop_t*, Callback) override;
    void cancel(Request*) override;

public:
    class Impl;
private:
    const std::unique_ptr<util::Thread<Impl>> thread;
    std::string accessToken;
};

}

#endif
