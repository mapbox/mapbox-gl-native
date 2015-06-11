#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <node.h>
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/resource.hpp>

#include <memory>

class NodeFileSource;

namespace node_mbgl {

class NodeRequest : public node::ObjectWrap {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);
    static NAN_METHOD(Respond);

    static v8::Handle<v8::Object> Create(v8::Handle<v8::Object> source, const mbgl::Resource& resource);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
public:
    NodeRequest(v8::Local<v8::Object> source, const mbgl::Resource& resource);
    ~NodeRequest();

    void cancel();

private:
    v8::Persistent<v8::Object> source;
    std::unique_ptr<mbgl::Resource> resource;
};

}
