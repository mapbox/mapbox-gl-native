#pragma once

#include <node.h>
#include <nan.h>

#include <thread>

class NodeFileSource;

class ThreadObject : public node::ObjectWrap {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
private:
    ThreadObject(NodeFileSource *fs);
    ~ThreadObject();

private:
    std::thread thread;
    v8::Persistent<v8::Object> self;
};
