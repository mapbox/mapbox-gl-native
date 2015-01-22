#ifndef MBGL_STORAGE_FILE_CACHE
#define MBGL_STORAGE_FILE_CACHE

#include <mbgl/util/noncopyable.hpp>

#include <functional>
#include <memory>

namespace mbgl {

struct Resource;
class Response;

class FileCache : private util::noncopyable {
public:
    virtual ~FileCache() = default;

    enum class Hint : uint8_t { Full, Refresh, No };

    virtual void get(const Resource &resource,
                     std::function<void(std::unique_ptr<Response>)> callback) = 0;
    virtual void put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) = 0;
};

}

#endif
