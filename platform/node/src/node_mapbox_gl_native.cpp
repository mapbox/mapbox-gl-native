#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <node.h>
#include <nan.h>
#pragma GCC diagnostic pop

#include "node_map.hpp"
#include "node_log.hpp"
#include "node_request.hpp"

namespace mbgl {

NAN_MODULE_INIT(RegisterModule) {
    NodeMap::Init(target);
    NodeRequest::Init(target);

    // Exports Resource constants.
    v8::Local<v8::Object> resource = Nan::New<v8::Object>();

    Nan::Set(resource,
        Nan::New("Unknown").ToLocalChecked(),
        Nan::New(Resource::Unknown));

    Nan::Set(resource,
        Nan::New("Style").ToLocalChecked(),
        Nan::New(Resource::Style));

    Nan::Set(resource,
        Nan::New("Source").ToLocalChecked(),
        Nan::New(Resource::Source));

    Nan::Set(resource,
        Nan::New("Tile").ToLocalChecked(),
        Nan::New(Resource::Tile));

    Nan::Set(resource,
        Nan::New("Glyphs").ToLocalChecked(),
        Nan::New(Resource::Glyphs));

    Nan::Set(resource,
        Nan::New("SpriteImage").ToLocalChecked(),
        Nan::New(Resource::SpriteImage));

    Nan::Set(resource,
        Nan::New("SpriteJSON").ToLocalChecked(),
        Nan::New(Resource::SpriteJSON));

    Nan::Set(target,
        Nan::New("Resource").ToLocalChecked(),
        resource);

    // Make the exported object inherit from process.EventEmitter
    v8::Local<v8::Object> process = Nan::Get(
        Nan::GetCurrentContext()->Global(),
        Nan::New("process").ToLocalChecked()).ToLocalChecked()->ToObject();

    v8::Local<v8::Object> EventEmitter = Nan::Get(process,
        Nan::New("EventEmitter").ToLocalChecked()).ToLocalChecked()->ToObject();

    Nan::SetPrototype(target,
        Nan::Get(EventEmitter, Nan::New("prototype").ToLocalChecked()).ToLocalChecked());

    Log::setObserver(std::make_unique<NodeLogObserver>(target->ToObject()));
}

NODE_MODULE(mapbox_gl_native, RegisterModule)

}
