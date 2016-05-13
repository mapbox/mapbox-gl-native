#ifndef MBGL_STORAGE_ASSET_REPOSITORY
#define MBGL_STORAGE_ASSET_REPOSITORY

#include <mbgl/storage/response.hpp>
#include <mbgl/storage/resource.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/async_request.hpp>

#include <functional>
#include <memory>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

namespace storage {

class AssetRepository : private util::noncopyable {
public:
    AssetRepository(const std::string& assetRoot);
    ~AssetRepository();

    using Callback = std::function<void (Response)>;
    std::unique_ptr<AsyncRequest> request(const Resource&, Callback);

private:
    class Impl;
    std::unique_ptr<util::Thread<Impl>> thread;
};

} // namespace storage
} // namespace mbgl

#endif // MBGL_STORAGE_ASSET_FILE_REPOSITORY
