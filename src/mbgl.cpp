#include <node.h>
#include <nan.h>

#include "node_file_source.hpp"
#include "node_map.hpp"
#include "node_request.hpp"

void RegisterModule(v8::Handle<v8::Object> exports) {
    NanScope();

    node_mbgl::NodeFileSource::Init(exports);
    node_mbgl::NodeMap::Init(exports);
    node_mbgl::NodeRequest::Init(exports);

    // Exports Resource constants.
    auto ConstantProperty = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    auto resource = v8::Object::New();
    resource->ForceSet(NanNew("Unknown"), NanNew(mbgl::Resource::Unknown), ConstantProperty);
    resource->ForceSet(NanNew("Tile"), NanNew(mbgl::Resource::Tile), ConstantProperty);
    resource->ForceSet(NanNew("Glyphs"), NanNew(mbgl::Resource::Glyphs), ConstantProperty);
    resource->ForceSet(NanNew("Image"), NanNew(mbgl::Resource::Image), ConstantProperty);
    resource->ForceSet(NanNew("JSON"), NanNew(mbgl::Resource::JSON), ConstantProperty);
    exports->ForceSet(NanNew("Resource"), resource, ConstantProperty);
}

NODE_MODULE(mbgl, RegisterModule)
