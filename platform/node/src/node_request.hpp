#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/file_source.hpp>

namespace node_mbgl {

class NodeRequest : public Nan::ObjectWrap, public Nan::AsyncWorker {
public:
    NodeRequest(mbgl::FileSource::Callback);
    ~NodeRequest();

    static Nan::Persistent<v8::Function> constructor;

    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(Respond);

    void Execute();

    static v8::Handle<v8::Object> Create(const mbgl::Resource&, mbgl::FileSource::Callback);

    struct NodeAsyncRequest : public mbgl::AsyncRequest {
        NodeAsyncRequest(NodeRequest*);
        ~NodeAsyncRequest() override;
        NodeRequest* request;
    };

private:
    mbgl::FileSource::Callback callback;
    NodeAsyncRequest* asyncRequest = nullptr;
};

}
