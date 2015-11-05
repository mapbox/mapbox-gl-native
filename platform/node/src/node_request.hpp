#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/resource.hpp>

#include <memory>

namespace node_mbgl {

class NodeFileSource;

class NodeRequest : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(Respond);

    static v8::Handle<v8::Object> Create(NodeFileSource*, const mbgl::Resource&);
    static Nan::Persistent<v8::Function> constructor;

    NodeRequest(NodeFileSource* source, const mbgl::Resource& resource);
    ~NodeRequest();

    void cancel();

private:
    NodeFileSource* source;
    std::unique_ptr<mbgl::Resource> resource;
};

}
