#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/file_source.hpp>

namespace node_mbgl {

class NodeFileSource;
class NodeRequest;

class NodeRequest : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(Respond);

    static v8::Handle<v8::Object> Create(const mbgl::Resource&, mbgl::FileSource::Callback);
    static Nan::Persistent<v8::Function> constructor;

    NodeRequest(mbgl::FileSource::Callback);
    ~NodeRequest();

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
