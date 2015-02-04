#pragma once

#include <node.h>
#include <nan.h>

class NodeFileSource;
namespace mbgl { class Request; }

namespace node_mbgl {

class NodeRequest : public node::ObjectWrap {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);
    static NAN_METHOD(Respond);

    static v8::Handle<v8::Object> Create(v8::Handle<v8::Object> source, mbgl::Request *request);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
public:
    NodeRequest(v8::Persistent<v8::Object> source, mbgl::Request *request);
    ~NodeRequest();

    void cancel();

private:
    v8::Persistent<v8::Object> source;
    mbgl::Request *request = nullptr;
};

}
