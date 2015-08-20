#include "node_map.hpp"

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/util/exception.hpp>

#include <unistd.h>

namespace node_mbgl {

struct NodeMap::RenderOptions {
    double zoom = 0;
    double bearing = 0;
    double latitude = 0;
    double longitude = 0;
    unsigned int width = 512;
    unsigned int height = 512;
    float ratio = 1.0f;
    std::vector<std::string> classes;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

v8::Persistent<v8::FunctionTemplate> NodeMap::constructorTemplate;

static std::shared_ptr<mbgl::HeadlessDisplay> sharedDisplay() {
    static auto display = std::make_shared<mbgl::HeadlessDisplay>();
    return display;
}

const static char* releasedMessage() {
    return "Map resources have already been released";
}

void NodeMap::Init(v8::Handle<v8::Object> target) {
    NanScope();

    v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(New);

    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(NanNew("Map"));

    NODE_SET_PROTOTYPE_METHOD(t, "load", Load);
    NODE_SET_PROTOTYPE_METHOD(t, "render", Render);
    NODE_SET_PROTOTYPE_METHOD(t, "release", Release);

    NanAssignPersistent(constructorTemplate, t);

    target->Set(NanNew("Map"), t->GetFunction());

    // Initialize display connection on module load.
    sharedDisplay();
}

NAN_METHOD(NodeMap::New) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowTypeError("Use the new operator to create new Map objects");
    }

    if (args.Length() < 1 || !args[0]->IsObject()) {
        return NanThrowTypeError("Requires an options object as first argument");
    }

    auto options = args[0]->ToObject();

    // Check that 'request', 'cancel' and 'ratio' are defined.
    if (!options->Has(NanNew("request")) || !options->Get(NanNew("request"))->IsFunction()) {
        return NanThrowError("Options object must have a 'request' method");
    }
    if (options->Has(NanNew("cancel")) && !options->Get(NanNew("cancel"))->IsFunction()) {
        return NanThrowError("Options object 'cancel' property must be a function");
    }

    if (!options->Has(NanNew("ratio")) || !options->Get(NanNew("ratio"))->IsNumber()) {
        return NanThrowError("Options object must have a numerical 'ratio' property");
    }

    try {
        auto nodeMap = new NodeMap(options);
        nodeMap->Wrap(args.This());
    } catch(std::exception &ex) {
        return NanThrowError(ex.what());
    }

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

    auto nodeMap = node::ObjectWrap::Unwrap<NodeMap>(args.Holder());

    if (!nodeMap->isValid()) return NanThrowError(releasedMessage());

    // Reset the flag as this could be the second time
    // we are calling this (being the previous successful).
    nodeMap->loaded = false;

    if (args.Length() < 1) {
        return NanThrowError("Requires a map style as first argument");
    }

    std::string style;

    if (args[0]->IsObject()) {
        style = StringifyStyle(args[0]);
    } else if (args[0]->IsString()) {
        style = *NanUtf8String(args[0]);
    } else {
        return NanThrowTypeError("First argument must be a string or object");
    }

    try {
        nodeMap->map->setStyleJSON(style, ".");
    } catch (const std::exception &ex) {
        return NanThrowError(ex.what());
    }

    nodeMap->loaded = true;

    NanReturnUndefined();
}

std::unique_ptr<NodeMap::RenderOptions> NodeMap::ParseOptions(v8::Local<v8::Object> obj) {
    NanScope();

    auto options = std::make_unique<RenderOptions>();

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

    auto nodeMap = node::ObjectWrap::Unwrap<NodeMap>(args.Holder());

    if (!nodeMap->isValid()) return NanThrowError(releasedMessage());

    if (args.Length() <= 0 || !args[0]->IsObject()) {
        return NanThrowTypeError("First argument must be an options object");
    }

    if (args.Length() <= 1 || !args[1]->IsFunction()) {
        return NanThrowTypeError("Second argument must be a callback function");
    }

    if (!nodeMap->isLoaded()) {
        return NanThrowTypeError("Style is not loaded");
    }

    auto options = ParseOptions(args[0]->ToObject());

    assert(!nodeMap->callback);
    assert(!nodeMap->image);
    nodeMap->callback = std::unique_ptr<NanCallback>(new NanCallback(args[1].As<v8::Function>()));

    try {
        nodeMap->startRender(std::move(options));
    } catch (mbgl::util::Exception &ex) {
        return NanThrowError(ex.what());
    }

    NanReturnUndefined();
}

void NodeMap::startRender(std::unique_ptr<NodeMap::RenderOptions> options) {
    view.resize(options->width, options->height);
    map->update(mbgl::Update::Dimensions);
    map->setClasses(options->classes);
    map->setLatLngZoom(mbgl::LatLng(options->latitude, options->longitude), options->zoom);
    map->setBearing(options->bearing);

    map->renderStill([this](const std::exception_ptr eptr, std::unique_ptr<const mbgl::StillImage> result) {
        if (eptr) {
            error = std::move(eptr);
            uv_async_send(async);
        } else {
            assert(!image);
            image = std::move(result);
            uv_async_send(async);
        }
    });

    // Retain this object, otherwise it might get destructed before we are finished rendering the
    // still image.
    Ref();

    // Similarly, we're now waiting for the async to be called, so we need to make sure that it
    // keeps the loop alive.
    uv_ref(reinterpret_cast<uv_handle_t *>(async));
}

void NodeMap::renderFinished() {
    NanScope();

    // We're done with this render call, so we're unrefing so that the loop could close.
    uv_unref(reinterpret_cast<uv_handle_t *>(async));

    // There is no render pending anymore, we the GC could now delete this object if it went out
    // of scope.
    Unref();

    // Move the callback and image out of the way so that the callback can start a new render call.
    auto cb = std::move(callback);
    auto img = std::move(image);
    assert(cb);

    // These have to be empty to be prepared for the next render call.
    assert(!callback);
    assert(!image);

    if (error) {
        std::string errorMessage;

        try {
            std::rethrow_exception(error);
        } catch (const std::exception& ex) {
            errorMessage = ex.what();
        }

        v8::Local<v8::Value> argv[] = {
            NanError(errorMessage.c_str())
        };

        // This must be empty to be prepared for the next render call.
        error = nullptr;
        assert(!error);

        cb->Call(1, argv);
    } else if (img) {
        auto result = NanNew<v8::Object>();
        result->Set(NanNew("width"), NanNew(img->width));
        result->Set(NanNew("height"), NanNew(img->height));

        v8::Local<v8::Object> pixels = NanNewBufferHandle(
            reinterpret_cast<char *>(img->pixels.get()),
            size_t(img->width) * size_t(img->height) * sizeof(mbgl::StillImage::Pixel),

            // Retain the StillImage object until the buffer is deleted.
            [](char *, void *hint) {
                delete reinterpret_cast<const mbgl::StillImage *>(hint);
            },
            const_cast<mbgl::StillImage *>(img.get())
        );
        img.release();

        result->Set(NanNew("pixels"), pixels);

        v8::Local<v8::Value> argv[] = {
            NanNull(),
            result,
        };
        cb->Call(2, argv);
    } else {
        v8::Local<v8::Value> argv[] = {
            NanError("Didn't get an image")
        };
        cb->Call(1, argv);
    }
}

NAN_METHOD(NodeMap::Release) {
    NanScope();

    auto nodeMap = node::ObjectWrap::Unwrap<NodeMap>(args.Holder());

    if (!nodeMap->isValid()) return NanThrowError(releasedMessage());

    try {
        nodeMap->release();
    } catch (const std::exception &ex) {
        return NanThrowError(ex.what());
    }

    NanReturnUndefined();
}

void NodeMap::release() {
    if (!isValid()) throw mbgl::util::Exception(releasedMessage());

    valid = false;

    uv_close(reinterpret_cast<uv_handle_t *>(async), [] (uv_handle_t *handle) {
        delete reinterpret_cast<uv_async_t *>(handle);
    });

    map.reset(nullptr);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

NodeMap::NodeMap(v8::Handle<v8::Object> options) :
    view(sharedDisplay(), options->Get(NanNew("ratio"))->NumberValue()),
    fs(options),
    map(std::make_unique<mbgl::Map>(view, fs, mbgl::MapMode::Still)),
    async(new uv_async_t) {

    async->data = this;
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    uv_async_init(uv_default_loop(), async, [](uv_async_t *as, int) {
#else
    uv_async_init(uv_default_loop(), async, [](uv_async_t *as) {
#endif
        reinterpret_cast<NodeMap *>(as->data)->renderFinished();
    });

    // Make sure the async handle doesn't keep the loop alive.
    uv_unref(reinterpret_cast<uv_handle_t *>(async));
}

NodeMap::~NodeMap() {
    if (valid) release();
}

}
