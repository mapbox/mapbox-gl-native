#pragma once

#include <mbgl/storage/file_source.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <nan.h>
#pragma GCC diagnostic pop

#include <unordered_map>
#include <mutex>

namespace mbgl {

namespace util { template <typename T> class AsyncQueue; }

class NodeFileSource : public FileSource {
public:
    NodeFileSource(v8::Local<v8::Object>);
    ~NodeFileSource() override;

    // FileSource API
    Request* request(const Resource&, uv_loop_t*, Callback) override;
    void cancel(Request*) override;

    void notify(const Resource&, const std::shared_ptr<const Response>&);

private:
    Nan::Persistent<v8::Object> options;

    struct Action;
    using Queue = util::AsyncQueue<Action>;

    Queue *queue = nullptr;

    void processAdd(const Resource&);
    void processCancel(const Resource&);

    std::unordered_map<Resource, Nan::Persistent<v8::Object>, Resource::Hash> pending;

    // The observers list will hold pointers to all the requests waiting
    // for a particular resource. The access must be guarded by a mutex
    // because the list is also accessed by a thread from the Map
    // object and from the main thread when notifying requests of
    // completion. Concurrent access is specially needed when
    // canceling a request to avoid a deadlock 
    // (see https://github.com/mapbox/node-mapbox-gl-native/issues/129).
    std::unordered_map<Resource, Request*, Resource::Hash> observers;
    std::mutex observersMutex;
};

}
