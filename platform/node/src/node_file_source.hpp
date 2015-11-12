#pragma once

#include <mbgl/storage/file_source.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <nan.h>
#pragma GCC diagnostic pop

namespace mbgl {

class NodeFileSource : public FileSource {
public:
    NodeFileSource(v8::Local<v8::Object>);
    ~NodeFileSource() override;

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

private:
    Nan::Persistent<v8::Object> options;
};

}
