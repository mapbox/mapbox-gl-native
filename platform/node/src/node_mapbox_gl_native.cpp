#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <node.h>
#include <nan.h>
#pragma GCC diagnostic pop

#include "node_map.hpp"
#include "node_log.hpp"
#include "node_request.hpp"

void RegisterModule(v8::Handle<v8::Object> exports) {
    NanScope();

    node_mbgl::NodeMap::Init(exports);
    node_mbgl::NodeRequest::Init(exports);

    // Exports Resource constants.
    auto ConstantProperty = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    auto resource = NanNew<v8::Object>();
    resource->ForceSet(NanNew("Unknown"), NanNew(mbgl::Resource::Unknown), ConstantProperty);
    resource->ForceSet(NanNew("Style"), NanNew(mbgl::Resource::Style), ConstantProperty);
    resource->ForceSet(NanNew("Source"), NanNew(mbgl::Resource::Source), ConstantProperty);
    resource->ForceSet(NanNew("Tile"), NanNew(mbgl::Resource::Tile), ConstantProperty);
    resource->ForceSet(NanNew("Glyphs"), NanNew(mbgl::Resource::Glyphs), ConstantProperty);
    resource->ForceSet(NanNew("SpriteImage"), NanNew(mbgl::Resource::SpriteImage), ConstantProperty);
    resource->ForceSet(NanNew("SpriteJSON"), NanNew(mbgl::Resource::SpriteJSON), ConstantProperty);
    exports->ForceSet(NanNew("Resource"), resource, ConstantProperty);

    // Make the exported object inerhit from process.EventEmitter
    auto process = NanGetCurrentContext()->Global()->Get(NanNew("process"))->ToObject();
    auto EventEmitter = process->Get(NanNew("EventEmitter"))->ToObject();
    exports->SetPrototype(EventEmitter->Get(NanNew("prototype")));

    mbgl::Log::setObserver(std::make_unique<node_mbgl::NodeLogObserver>(exports));
}

NODE_MODULE(mapbox_gl_native, RegisterModule)
