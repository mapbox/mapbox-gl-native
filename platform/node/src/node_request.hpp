#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/util/async_request.hpp>

namespace node_mbgl {

class NodeRequest;

struct NodeAsyncRequest : public mbgl::AsyncRequest {
    NodeAsyncRequest();
    ~NodeAsyncRequest() override;
    NodeRequest* request = nullptr;
};

class NodeRequest : public Nan::ObjectWrap {

public:

    NodeRequest(mbgl::FileSource::Callback, NodeAsyncRequest*);
    ~NodeRequest() override;

    static Nan::Persistent<v8::Function> constructor;

    static void Init();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void HandleCallback(const Nan::FunctionCallbackInfo<v8::Value>&);

    void unrefRequest();

    mbgl::FileSource::Callback callback;
    NodeAsyncRequest* asyncRequest;
    Nan::AsyncResource* asyncResource = new Nan::AsyncResource("mbgl:execute");
};

} // namespace node_mbgl
