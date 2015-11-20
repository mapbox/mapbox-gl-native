#include "node_map.hpp"

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/util/exception.hpp>

#include <unistd.h>

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
#define UV_ASYNC_PARAMS(handle) uv_async_t *handle, int
#else
#define UV_ASYNC_PARAMS(handle) uv_async_t *handle
#endif

namespace mbgl {

struct NodeMap::RenderOptions {
    double zoom = 0;
    double bearing = 0;
    double pitch = 0;
    double latitude = 0;
    double longitude = 0;
    unsigned int width = 512;
    unsigned int height = 512;
    std::vector<std::string> classes;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

Nan::Persistent<v8::Function> NodeMap::constructor;

static std::shared_ptr<HeadlessDisplay> sharedDisplay() {
    static auto display = std::make_shared<HeadlessDisplay>();
    return display;
}

const static char* releasedMessage() {
    return "Map resources have already been released";
}

NAN_MODULE_INIT(NodeMap::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Map").ToLocalChecked());

    Nan::SetPrototypeMethod(tpl, "load", Load);
    Nan::SetPrototypeMethod(tpl, "render", Render);
    Nan::SetPrototypeMethod(tpl, "release", Release);
    Nan::SetPrototypeMethod(tpl, "dumpDebugLogs", DumpDebugLogs);

    constructor.Reset(tpl->GetFunction());
    Nan::Set(target, Nan::New("Map").ToLocalChecked(), tpl->GetFunction());

    // Initialize display connection on module load.
    sharedDisplay();
}

/**
 * A request object, given to the `request` handler of a map, is an
 * encapsulation of a URL and type of a resource that the map asks you to load.
 *
 * The `kind` property is one of
 *
 *     "Unknown": 0,
 *     "Style": 1,
 *     "Source": 2,
 *     "Tile": 3,
 *     "Glyphs": 4,
 *     "SpriteImage": 5,
 *     "SpriteJSON": 6
 *
 * @typedef
 * @name Request
 * @property {string} url
 * @property {number} kind
 */

/**
 * Mapbox GL object: this object loads stylesheets and renders them into
 * images.
 *
 * @class
 * @name Map
 * @param {Object} options
 * @param {Function} options.request a method used to request resources
 * over the internet
 * @param {Function} [options.cancel]
 * @param {number} options.ratio pixel ratio
 * @example
 * var map = new mbgl.Map({ request: function() {} });
 * map.load(require('./test/fixtures/style.json'));
 * map.render({}, function(err, image) {
 *     if (err) throw err;
 *     fs.writeFileSync('image.png', image);
 * });
 */
NAN_METHOD(NodeMap::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new Map objects");
    }

    if (info.Length() < 1 || !info[0]->IsObject()) {
        return Nan::ThrowTypeError("Requires an options object as first argument");
    }

    auto options = info[0]->ToObject();

    // Check that 'request' is set. If 'cancel' is set it must be a
    // function and if 'ratio' is set it must be a number.
    if (!Nan::Has(options, Nan::New("request").ToLocalChecked()).FromJust()
     || !Nan::Get(options, Nan::New("request").ToLocalChecked()).ToLocalChecked()->IsFunction()) {
        return Nan::ThrowError("Options object must have a 'request' method");
    }

    if (Nan::Has(options, Nan::New("cancel").ToLocalChecked()).FromJust()
     && !Nan::Get(options, Nan::New("cancel").ToLocalChecked()).ToLocalChecked()->IsFunction()) {
        return Nan::ThrowError("Options object 'cancel' property must be a function");
    }

    if (Nan::Has(options, Nan::New("ratio").ToLocalChecked()).FromJust()
     && !Nan::Get(options, Nan::New("ratio").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
        return Nan::ThrowError("Options object 'ratio' property must be a number");
    }

    try {
        auto nodeMap = new NodeMap(options);
        nodeMap->Wrap(info.This());
    } catch(std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().Set(info.This());
}

const std::string StringifyStyle(v8::Local<v8::Value> styleHandle) {
    Nan::HandleScope scope;

    v8::Local<v8::Object> JSON = Nan::Get(
        Nan::GetCurrentContext()->Global(),
        Nan::New("JSON").ToLocalChecked()).ToLocalChecked()->ToObject();

    return *Nan::Utf8String(Nan::MakeCallback(JSON, "stringify", 1, &styleHandle));
}

/**
 * Load a stylesheet
 *
 * @function
 * @name load
 * @param {string|Object} stylesheet either an object or a JSON representation
 * @returns {undefined} loads stylesheet into map
 * @throws {Error} if stylesheet is missing or invalid
 * @example
 * // providing an object
 * map.load(require('./test/fixtures/style.json'));
 *
 * // providing a string
 * map.load(fs.readFileSync('./test/fixtures/style.json', 'utf8'));
 */
NAN_METHOD(NodeMap::Load) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());

    if (!nodeMap->isValid()) return Nan::ThrowError(releasedMessage());

    // Reset the flag as this could be the second time
    // we are calling this (being the previous successful).
    nodeMap->loaded = false;

    if (info.Length() < 1) {
        return Nan::ThrowError("Requires a map style as first argument");
    }

    std::string style;

    if (info[0]->IsObject()) {
        style = StringifyStyle(info[0]);
    } else if (info[0]->IsString()) {
        style = *Nan::Utf8String(info[0]);
    } else {
        return Nan::ThrowTypeError("First argument must be a string or object");
    }

    try {
        nodeMap->map->setStyleJSON(style, ".");
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    nodeMap->loaded = true;

    info.GetReturnValue().SetUndefined();
}

std::unique_ptr<NodeMap::RenderOptions> NodeMap::ParseOptions(v8::Local<v8::Object> obj) {
    Nan::HandleScope scope;

    auto options = std::make_unique<RenderOptions>();

    if (Nan::Has(obj, Nan::New("zoom").ToLocalChecked()).FromJust()) {
        options->zoom = Nan::Get(obj, Nan::New("zoom").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if (Nan::Has(obj, Nan::New("bearing").ToLocalChecked()).FromJust()) {
        options->bearing = Nan::Get(obj, Nan::New("bearing").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if (Nan::Has(obj, Nan::New("pitch").ToLocalChecked()).FromJust()) {
        options->pitch = Nan::Get(obj, Nan::New("pitch").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if (Nan::Has(obj, Nan::New("center").ToLocalChecked()).FromJust()) {
        auto center = Nan::Get(obj, Nan::New("center").ToLocalChecked()).ToLocalChecked().As<v8::Array>();
        if (center->Length() > 0) { options->longitude = Nan::Get(center, 0).ToLocalChecked()->NumberValue(); }
        if (center->Length() > 1) { options->latitude = Nan::Get(center, 1).ToLocalChecked()->NumberValue(); }
    }

    if (Nan::Has(obj, Nan::New("width").ToLocalChecked()).FromJust()) {
        options->width = Nan::Get(obj, Nan::New("width").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    }

    if (Nan::Has(obj, Nan::New("height").ToLocalChecked()).FromJust()) {
        options->height = Nan::Get(obj, Nan::New("height").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    }

    if (Nan::Has(obj, Nan::New("classes").ToLocalChecked()).FromJust()) {
        auto classes = Nan::Get(obj, Nan::New("classes").ToLocalChecked()).ToLocalChecked()->ToObject().As<v8::Array>();
        const int length = classes->Length();
        options->classes.reserve(length);
        for (int i = 0; i < length; i++) {
            options->classes.push_back(std::string { *Nan::Utf8String(Nan::Get(classes, i).ToLocalChecked()->ToString()) });
        }
    }

    return options;
}

/**
 * Render an image from the currently-loaded style
 *
 * @name render
 * @param {Object} options
 * @param {number} [options.zoom=0]
 * @param {number} [options.width=512]
 * @param {number} [options.height=512]
 * @param {Array<number>} [options.center=[0,0]] longitude, latitude center
 * of the map
 * @param {number} [options.bearing=0] rotation
 * @param {Array<string>} [options.classes=[]] GL Style Classes
 * @param {Function} callback
 * @returns {undefined} calls callback
 * @throws {Error} if stylesheet is not loaded or if map is already rendering
 */
NAN_METHOD(NodeMap::Render) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());

    if (!nodeMap->isValid()) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an options object");
    }

    if (info.Length() <= 1 || !info[1]->IsFunction()) {
        return Nan::ThrowTypeError("Second argument must be a callback function");
    }

    if (!nodeMap->isLoaded()) {
        return Nan::ThrowTypeError("Style is not loaded");
    }

    if (nodeMap->callback) {
        return Nan::ThrowError("Map is currently rendering an image");
    }

    auto options = ParseOptions(info[0]->ToObject());

    assert(!nodeMap->callback);
    assert(!nodeMap->image);
    nodeMap->callback = std::make_unique<Nan::Callback>(info[1].As<v8::Function>());

    try {
        nodeMap->startRender(std::move(options));
    } catch (util::Exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::startRender(std::unique_ptr<NodeMap::RenderOptions> options) {
    view.resize(options->width, options->height);
    map->update(Update::Dimensions);
    map->setClasses(options->classes);
    map->setLatLngZoom(LatLng(options->latitude, options->longitude), options->zoom);
    map->setBearing(options->bearing);
    map->setPitch(options->pitch);

    map->renderStill([this](const std::exception_ptr eptr, std::unique_ptr<const StillImage> result) {
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
    Nan::HandleScope scope;

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
            Nan::Error(errorMessage.c_str())
        };

        // This must be empty to be prepared for the next render call.
        error = nullptr;
        assert(!error);

        cb->Call(1, argv);
    } else if (img) {
        v8::Local<v8::Object> pixels = Nan::NewBuffer(
            reinterpret_cast<char *>(img->pixels.get()),
            size_t(img->width) * size_t(img->height) * sizeof(StillImage::Pixel),

            // Retain the StillImage object until the buffer is deleted.
            [](char *, void *hint) {
                delete reinterpret_cast<const StillImage *>(hint);
            },
            const_cast<StillImage *>(img.get())
        ).ToLocalChecked();
        img.release();

        v8::Local<v8::Value> argv[] = {
            Nan::Null(),
            pixels
        };
        cb->Call(2, argv);
    } else {
        v8::Local<v8::Value> argv[] = {
            Nan::Error("Didn't get an image")
        };
        cb->Call(1, argv);
    }
}

/**
 * Clean up any resources used by a map instance.options
 * @name release
 * @returns {undefined}
 */
NAN_METHOD(NodeMap::Release) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());

    if (!nodeMap->isValid()) return Nan::ThrowError(releasedMessage());

    try {
        nodeMap->release();
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::release() {
    if (!isValid()) throw util::Exception(releasedMessage());

    valid = false;

    uv_close(reinterpret_cast<uv_handle_t *>(async), [] (uv_handle_t *handle) {
        delete reinterpret_cast<uv_async_t *>(handle);
    });

    map.reset(nullptr);
}

NAN_METHOD(NodeMap::DumpDebugLogs) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());

    if (!nodeMap->isValid()) return Nan::ThrowError(releasedMessage());

    nodeMap->map->dumpDebugLogs();
    info.GetReturnValue().SetUndefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

NodeMap::NodeMap(v8::Local<v8::Object> options) :
    view(sharedDisplay(), [&] {
        Nan::HandleScope scope;
        return Nan::Has(options, Nan::New("ratio").ToLocalChecked()).FromJust() ? Nan::Get(options, Nan::New("ratio").ToLocalChecked()).ToLocalChecked()->NumberValue() : 1.0;
    }()),
    fs(options),
    map(std::make_unique<Map>(view, fs, MapMode::Still)),
    async(new uv_async_t) {

    async->data = this;
    uv_async_init(uv_default_loop(), async, [](UV_ASYNC_PARAMS(handle)) {
        reinterpret_cast<NodeMap *>(handle->data)->renderFinished();
    });

    // Make sure the async handle doesn't keep the loop alive.
    uv_unref(reinterpret_cast<uv_handle_t *>(async));
}

NodeMap::~NodeMap() {
    if (valid) release();
}

}
