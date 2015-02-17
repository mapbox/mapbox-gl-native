#ifndef MBGL_STORAGE_DEFAULT_SHARED_REQUEST_BASE
#define MBGL_STORAGE_DEFAULT_SHARED_REQUEST_BASE

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/file_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <set>
#include <cassert>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class Request;
class Response;
class DefaultFileSource;

class SharedRequestBase : private util::noncopyable {
protected:
    MBGL_STORE_THREAD(tid)

public:
    SharedRequestBase(DefaultFileSource *source_, const Resource &resource_)
        : resource(resource_), source(source_) {}

    virtual void start(uv_loop_t *loop, std::unique_ptr<Response> response = nullptr) = 0;
    virtual void cancel() = 0;

    void notify(std::unique_ptr<Response> response, FileCache::Hint hint) {
        MBGL_VERIFY_THREAD(tid);

        if (source) {
            source->notify(this, observers, std::shared_ptr<const Response>(response.release()),
                           hint);
        }
    }

    void subscribe(Request *request) {
        MBGL_VERIFY_THREAD(tid);

        observers.insert(request);
    }

    void unsubscribeAll() {
        MBGL_VERIFY_THREAD(tid);

        source = nullptr;
        observers.clear();
        cancel();
    }

    void unsubscribe(Request *request) {
        MBGL_VERIFY_THREAD(tid);

        observers.erase(request);

        if (observers.empty()) {
            // There are no observers anymore. We are initiating cancelation.
            if (source) {
                // First, remove this SharedRequestBase from the source.
                source->notify(this, observers, nullptr, FileCache::Hint::No);
            }

            // Then, initiate cancelation of this request
            cancel();
        }
    }

protected:
    virtual ~SharedRequestBase() {
        MBGL_VERIFY_THREAD(tid);
    }

public:
    const Resource resource;

protected:
    DefaultFileSource *source = nullptr;

private:
    std::set<Request *> observers;
};

}

#endif
