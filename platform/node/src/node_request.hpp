#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/file_source.hpp>

namespace node_mbgl {

class NodeFileSource;

class NodeRequest : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(Respond);

    static v8::Handle<v8::Object> Create(const mbgl::Resource&, mbgl::FileSource::Callback);
    static Nan::Persistent<v8::Function> constructor;

    NodeRequest(mbgl::FileSource::Callback);

private:
    mbgl::FileSource::Callback callback;
};

}
