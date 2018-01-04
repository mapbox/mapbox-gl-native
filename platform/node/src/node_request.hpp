#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/file_source.hpp>

namespace node_mbgl {

class NodeMap;

class NodeRequest : public Nan::ObjectWrap,
                    public Nan::AsyncWorker {
public:
    struct NodeAsyncRequest : public mbgl::AsyncRequest {
        NodeAsyncRequest(NodeRequest*);
        ~NodeAsyncRequest() override;
        NodeRequest* request;
    };

    NodeRequest(NodeMap*, mbgl::FileSource::Callback);
    ~NodeRequest();

    static Nan::Persistent<v8::Function> constructor;

    static void Init();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void HandleCallback(const Nan::FunctionCallbackInfo<v8::Value>&);

    void Execute();

private:
    NodeMap* target;
    mbgl::FileSource::Callback callback;
    NodeAsyncRequest* asyncRequest = nullptr;
};

}
