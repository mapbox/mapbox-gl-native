#ifndef MBGL_STORAGE_FILE_CACHE
#define MBGL_STORAGE_FILE_CACHE

#include <mbgl/util/noncopyable.hpp>

#include <functional>
#include <memory>

namespace mbgl {

struct Resource;
class Response;
class WorkRequest;

class FileCache : private util::noncopyable {
public:
    virtual ~FileCache() = default;

    enum class Hint : uint8_t { Full, Refresh, No };
    using Callback = std::function<void(std::unique_ptr<Response>)>;

    virtual std::unique_ptr<WorkRequest> get(const Resource &resource, Callback callback) = 0;
    virtual void put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) = 0;
};

}

#endif
