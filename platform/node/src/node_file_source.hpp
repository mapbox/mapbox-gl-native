#pragma once

#include <mbgl/storage/file_source.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

class NodeFileSource : public mbgl::FileSource {
public:
    NodeFileSource(v8::Local<v8::Object>);
    ~NodeFileSource();

    std::unique_ptr<mbgl::FileRequest> request(const mbgl::Resource&, Callback);

private:
    Nan::Persistent<v8::Object> options;
};

}
