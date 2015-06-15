#pragma once

#include <mbgl/storage/file_source.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <node.h>
#include <node_version.h>
#include <nan.h>
#pragma GCC diagnostic pop

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace node_mbgl {

namespace util { template <typename T> class AsyncQueue; }

class NodeFileSource : public node::ObjectWrap, public mbgl::FileSource {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
public:
    NodeFileSource();
    ~NodeFileSource();

    mbgl::Request* request(const mbgl::Resource&, uv_loop_t*, Callback);
    void cancel(mbgl::Request*);

    // visiblity?
    void notify(const mbgl::Resource&, const std::shared_ptr<const mbgl::Response>&);

private:
    struct Action;
    using Queue = util::AsyncQueue<Action>;

    void processAdd(const mbgl::Resource&);
    void processCancel(const mbgl::Resource&);

private:
#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
    std::unordered_map<mbgl::Resource, v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>>, mbgl::Resource::Hash> pending;
#else
    std::unordered_map<mbgl::Resource, v8::Persistent<v8::Object>, mbgl::Resource::Hash> pending;
#endif

    // The observers list will hold pointers to all the requests waiting
    // for a particular resource. It is also used for coalescing requests,
    // so we don't ask for the same resources twice. The access must be
    // guarded by a mutex because the list is also accessed by a thread
    // from the mbgl::Map object and from the main thread when notifying
    // requests of completion. Concurrent access is specially needed when
    // canceling a request to avoid a deadlock (see #129).
    std::unordered_map<mbgl::Resource, std::vector<mbgl::Request*>, mbgl::Resource::Hash> observers;
    std::mutex observersMutex;

    Queue *queue = nullptr;
};

}
