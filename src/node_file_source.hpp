#pragma once

#include <mbgl/storage/file_source.hpp>

#include <node.h>
#include <nan.h>

#include <map>

namespace mbgl { namespace util { template <typename T> class AsyncQueue; } }

namespace node_mbgl {

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

    mbgl::Request *request(const mbgl::Resource &resource, uv_loop_t *loop, const mbgl::Environment &env, Callback callback);
    void request(const mbgl::Resource &resource, const mbgl::Environment &env, Callback callback);
    void cancel(mbgl::Request *request);
    void abort(const mbgl::Environment &env);


    // visiblity?
    void notify(mbgl::Request *request, const std::shared_ptr<const mbgl::Response> &response);

private:
    struct Action;
    using Queue = mbgl::util::AsyncQueue<Action>;

    void processAdd(mbgl::Request *request);
    void processCancel(mbgl::Request *request);

private:
    std::map<mbgl::Request *, v8::Persistent<v8::Object>> pending;
    Queue *queue = nullptr;
};

}
