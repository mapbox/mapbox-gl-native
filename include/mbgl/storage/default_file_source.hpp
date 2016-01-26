#ifndef MBGL_STORAGE_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class DefaultFileSource : public FileSource {
public:
    DefaultFileSource(const std::string& cachePath, const std::string& assetRoot);
    ~DefaultFileSource() override;

    void setAccessToken(const std::string&);
    std::string getAccessToken() const;

    void setMaximumCacheSize(uint64_t size);
    void setMaximumCacheEntrySize(uint64_t size);

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

    // For testing only.
    void put(const Resource&, const Response&);
    void goOffline();

    class Impl;

private:
    const std::unique_ptr<util::Thread<Impl>> thread;
    const std::unique_ptr<FileSource> assetFileSource;
};

} // namespace mbgl

#endif
