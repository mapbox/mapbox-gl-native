#ifndef MBGL_STORAGE_ASSET_FILE_SOURCE
#define MBGL_STORAGE_ASSET_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class AssetFileSource : public FileSource {
public:
    AssetFileSource(const std::string& assetRoot);
    ~AssetFileSource() override;

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

private:
    class Impl;
    std::unique_ptr<util::Thread<Impl>> thread;
};

} // namespace mbgl

#endif // MBGL_STORAGE_ASSET_FILE_SOURCE
