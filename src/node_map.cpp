#include "node_map.hpp"
#include "node_map_render_worker.hpp"
#include "node_file_source.hpp"

#include <mbgl/platform/default/headless_display.hpp>

 #include <unistd.h>

namespace node_mbgl {

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

v8::Persistent<v8::FunctionTemplate> NodeMap::constructorTemplate;

void NodeMap::Init(v8::Handle<v8::Object> target) {
    NanScope();

    v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(New);

    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(NanNew("Map"));

    NODE_SET_PROTOTYPE_METHOD(t, "load", Load);
    NODE_SET_PROTOTYPE_METHOD(t, "render", Render);

    NanAssignPersistent(constructorTemplate, t);

    target->Set(NanNew("Map"), t->GetFunction());
}

NAN_METHOD(NodeMap::New) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowTypeError("Use the new operator to create new Map objects");
    }

    if (args.Length() < 1 || !NanHasInstance(NodeFileSource::constructorTemplate, args[0])) {
        return NanThrowTypeError("Requires a FileSource as first argument");
    }

    auto source = args[0]->ToObject();

    // Check that request() and cancel() are defined.
    if (!source->Has(NanNew("request")) || !source->Get(NanNew("request"))->IsFunction()) {
        return NanThrowError("FileSource must have a request member function");
    }
    if (!source->Has(NanNew("cancel")) || !source->Get(NanNew("cancel"))->IsFunction()) {
        return NanThrowError("FileSource must have a cancel member function");
    }

    auto map = new NodeMap(args[0]->ToObject());
    map->Wrap(args.This());

    NanReturnValue(args.This());
}

const std::string StringifyStyle(v8::Handle<v8::Value> styleHandle) {
    NanScope();

    auto JSON = NanGetCurrentContext()->Global()->Get(NanNew("JSON"))->ToObject();
    auto stringify = v8::Handle<v8::Function>::Cast(JSON->Get(NanNew("stringify")));

    return *NanUtf8String(stringify->Call(JSON, 1, &styleHandle));
}

NAN_METHOD(NodeMap::Load) {
    NanScope();

    if (args.Length() != 1) {
        return NanThrowError("Requires a map style as first argument");
    }

    std::string style;

    if (args[0]->IsObject()) {
        style = StringifyStyle(args[0]);
    } else if (args[0]->IsString()) {
        v8::Local<v8::String> toStr = args[0]->ToString();
        style.resize(toStr->Utf8Length());
        toStr->WriteUtf8(const_cast<char *>(style.data()));
    } else {
        return NanThrowTypeError("First argument must be a string or object");
    }

    auto nodeMap = node::ObjectWrap::Unwrap<NodeMap>(args.Holder());

    try {
        nodeMap->map.setStyleJSON(style, ".");
    } catch (const std::exception &ex) {
        NanThrowError(ex.what());
    }

    NanReturnUndefined();
}

std::unique_ptr<NodeMap::RenderOptions> NodeMap::ParseOptions(v8::Local<v8::Object> obj) {
    NanScope();

    auto options = mbgl::util::make_unique<RenderOptions>();

    if (obj->Has(NanNew("zoom"))) { options->zoom = obj->Get(NanNew("zoom"))->NumberValue(); }
    if (obj->Has(NanNew("bearing"))) { options->bearing = obj->Get(NanNew("bearing"))->NumberValue(); }
    if (obj->Has(NanNew("center"))) {
        auto center = obj->Get(NanNew("center")).As<v8::Array>();
        if (center->Length() > 0) { options->latitude = center->Get(0)->NumberValue(); }
        if (center->Length() > 1) { options->longitude = center->Get(1)->NumberValue(); }
    }
    if (obj->Has(NanNew("width"))) { options->width = obj->Get(NanNew("width"))->IntegerValue(); }
    if (obj->Has(NanNew("height"))) { options->height = obj->Get(NanNew("height"))->IntegerValue(); }
    if (obj->Has(NanNew("ratio"))) { options->ratio = obj->Get(NanNew("ratio"))->IntegerValue(); }
    if (obj->Has(NanNew("accessToken"))) {
        options->accessToken = *NanUtf8String(obj->Get(NanNew("accessToken")->ToString()));
    }

    if (obj->Has(NanNew("classes"))) {
        auto classes = obj->Get(NanNew("classes"))->ToObject().As<v8::Array>();
        const int length = classes->Length();
        options->classes.reserve(length);
        for (int i = 0; i < length; i++) {
            options->classes.push_back(std::string { *NanUtf8String(classes->Get(i)->ToString()) });
        }
    }

    return options;
}

NAN_METHOD(NodeMap::Render) {
    NanScope();

    if (args.Length() <= 0 || !args[0]->IsObject()) {
        return NanThrowTypeError("First argument must be an options object");
    }

    if (args.Length() <= 1 || !args[1]->IsFunction()) {
        return NanThrowTypeError("Second argument must be a callback function");
    }

    auto nodeMap = node::ObjectWrap::Unwrap<NodeMap>(args.Holder());

    const bool wasEmpty = nodeMap->queue_.empty();

    nodeMap->queue_.push(mbgl::util::make_unique<RenderWorker>(
        nodeMap,
        ParseOptions(args[0]->ToObject()),
        new NanCallback(args[1].As<v8::Function>())));

    if (wasEmpty) {
        // When the queue was empty, there was no action in progress, so we can start a new one.
        NanAsyncQueueWorker(nodeMap->queue_.front().release());
    }

    NanReturnUndefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

std::shared_ptr<mbgl::HeadlessDisplay> sharedDisplay() {
    static auto display = std::make_shared<mbgl::HeadlessDisplay>();
    return display;
}

NodeMap::NodeMap(v8::Handle<v8::Object> source_) :
    view(sharedDisplay()),
    fs(*ObjectWrap::Unwrap<NodeFileSource>(source_)),
    map(view, fs) {
    source = v8::Persistent<v8::Object>::New(source_);
}

NodeMap::~NodeMap() {
    source.Dispose();
}

void NodeMap::processNext() {
    assert(!queue_.empty());
    queue_.pop();
    if (!queue_.empty()) {
        // When the queue was empty, there was no action in progress, so we can start a new one.
        NanAsyncQueueWorker(queue_.front().release());
    }
}

}
