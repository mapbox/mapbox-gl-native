#ifndef MBGL_STORAGE_DEFAULT_REQUEST
#define MBGL_STORAGE_DEFAULT_REQUEST

#include <mbgl/storage/resource.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>

#include <thread>
#include <functional>
#include <memory>

namespace mbgl {

class Response;

class Request : private util::noncopyable {
public:
    using Callback = std::function<void(const Response &)>;
    Request(const Resource &resource, std::thread::id tid, uint32_t env, Callback callback);

    // May be called from any thread.
    void notify(const std::shared_ptr<const Response> &response);
    void destruct();
    void cancel();

private:
    ~Request() = default;
    void invoke();
    void notifyCallback();

    struct Canceled;
    std::unique_ptr<Canceled> canceled;

    Callback callback;
    std::shared_ptr<const Response> response;
    std::thread::id tid;

public:
    const Resource resource;

    // The environment ID is used to associate requests with a particular environment. This allows
    // us to only terminate requests associated with that environment, e.g. when the map the env
    // belongs to is discarded.
    uint32_t env;
};

}

#endif
