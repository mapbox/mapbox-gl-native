#include "node_map.hpp"
#include "node_request.hpp"
#include "node_feature.hpp"
#include "node_conversion.hpp"
#include "node_geojson.hpp"

#include <mbgl/gl/headless_display.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/map/query.hpp>
#include <mbgl/util/premultiply.hpp>

#include <unistd.h>

namespace node_mbgl {

struct NodeMap::RenderOptions {
    double zoom = 0;
    double bearing = 0;
    double pitch = 0;
    double latitude = 0;
    double longitude = 0;
    unsigned int width = 512;
    unsigned int height = 512;
    std::vector<std::string> classes;
    mbgl::MapDebugOptions debugOptions = mbgl::MapDebugOptions::NoDebug;
};

Nan::Persistent<v8::Function> NodeMap::constructor;

static std::shared_ptr<mbgl::HeadlessDisplay> sharedDisplay() {
    static auto display = std::make_shared<mbgl::HeadlessDisplay>();
    return display;
}

static const char* releasedMessage() {
    return "Map resources have already been released";
}

NodeBackend::NodeBackend()
    : HeadlessBackend(sharedDisplay()) {}

void NodeBackend::onDidFailLoadingMap(std::exception_ptr error) {
    std::rethrow_exception(error);
}

void NodeMap::Init(v8::Local<v8::Object> target) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

    tpl->SetClassName(Nan::New("Map").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(2);

    Nan::SetPrototypeMethod(tpl, "load", Load);
    Nan::SetPrototypeMethod(tpl, "loaded", Loaded);
    Nan::SetPrototypeMethod(tpl, "render", Render);
    Nan::SetPrototypeMethod(tpl, "release", Release);
    Nan::SetPrototypeMethod(tpl, "cancel", Cancel);

    Nan::SetPrototypeMethod(tpl, "addSource", AddSource);
    Nan::SetPrototypeMethod(tpl, "addLayer", AddLayer);
    Nan::SetPrototypeMethod(tpl, "removeLayer", RemoveLayer);
    Nan::SetPrototypeMethod(tpl, "addImage", AddImage);
    Nan::SetPrototypeMethod(tpl, "removeImage", RemoveImage);
    Nan::SetPrototypeMethod(tpl, "setLayoutProperty", SetLayoutProperty);
    Nan::SetPrototypeMethod(tpl, "setPaintProperty", SetPaintProperty);
    Nan::SetPrototypeMethod(tpl, "setFilter", SetFilter);
    Nan::SetPrototypeMethod(tpl, "setCenter", SetCenter);
    Nan::SetPrototypeMethod(tpl, "setZoom", SetZoom);
    Nan::SetPrototypeMethod(tpl, "setBearing", SetBearing);
    Nan::SetPrototypeMethod(tpl, "setPitch", SetPitch);

    Nan::SetPrototypeMethod(tpl, "dumpDebugLogs", DumpDebugLogs);
    Nan::SetPrototypeMethod(tpl, "queryRenderedFeatures", QueryRenderedFeatures);

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
void NodeMap::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new Map objects");
    }

    if (info.Length() < 1 || !info[0]->IsObject()) {
        return Nan::ThrowTypeError("Requires an options object as first argument");
    }

    auto options = Nan::To<v8::Object>(info[0]).ToLocalChecked();

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

    info.This()->SetInternalField(1, options);

    try {
        auto nodeMap = new NodeMap(options);
        nodeMap->Wrap(info.This());
    } catch(std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().Set(info.This());
}

std::string StringifyStyle(v8::Local<v8::Value> styleHandle) {
    Nan::HandleScope scope;

    v8::Local<v8::Object> JSON = Nan::To<v8::Object>(
        Nan::Get(
            Nan::GetCurrentContext()->Global(),
            Nan::New("JSON").ToLocalChecked()
        ).ToLocalChecked()
    ).ToLocalChecked();

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
void NodeMap::Load(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

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
        nodeMap->map->setStyleJSON(style);
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    nodeMap->loaded = true;

    info.GetReturnValue().SetUndefined();
}

void NodeMap::Loaded(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    bool loaded = false;

    try {
        loaded = nodeMap->map->isFullyLoaded();
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().Set(Nan::New(loaded));
}

NodeMap::RenderOptions NodeMap::ParseOptions(v8::Local<v8::Object> obj) {
    Nan::HandleScope scope;

    NodeMap::RenderOptions options;

    if (Nan::Has(obj, Nan::New("zoom").ToLocalChecked()).FromJust()) {
        options.zoom = Nan::Get(obj, Nan::New("zoom").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if (Nan::Has(obj, Nan::New("bearing").ToLocalChecked()).FromJust()) {
        options.bearing = Nan::Get(obj, Nan::New("bearing").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if (Nan::Has(obj, Nan::New("pitch").ToLocalChecked()).FromJust()) {
        options.pitch = Nan::Get(obj, Nan::New("pitch").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if (Nan::Has(obj, Nan::New("center").ToLocalChecked()).FromJust()) {
        auto centerObj = Nan::Get(obj, Nan::New("center").ToLocalChecked()).ToLocalChecked();
        if (centerObj->IsArray()) {
            auto center = centerObj.As<v8::Array>();
            if (center->Length() > 0) { options.longitude = Nan::Get(center, 0).ToLocalChecked()->NumberValue(); }
            if (center->Length() > 1) { options.latitude = Nan::Get(center, 1).ToLocalChecked()->NumberValue(); }
        }
    }

    if (Nan::Has(obj, Nan::New("width").ToLocalChecked()).FromJust()) {
        options.width = Nan::Get(obj, Nan::New("width").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    }

    if (Nan::Has(obj, Nan::New("height").ToLocalChecked()).FromJust()) {
        options.height = Nan::Get(obj, Nan::New("height").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    }

    if (Nan::Has(obj, Nan::New("classes").ToLocalChecked()).FromJust()) {
        auto classes = Nan::To<v8::Object>(Nan::Get(obj, Nan::New("classes").ToLocalChecked()).ToLocalChecked()).ToLocalChecked().As<v8::Array>();
        const int length = classes->Length();
        options.classes.reserve(length);
        for (int i = 0; i < length; i++) {
            options.classes.emplace_back(std::string { *Nan::Utf8String(Nan::To<v8::String>(Nan::Get(classes, i).ToLocalChecked()).ToLocalChecked()) });
        }
    }

    if (Nan::Has(obj, Nan::New("debug").ToLocalChecked()).FromJust()) {
        auto debug = Nan::To<v8::Object>(Nan::Get(obj, Nan::New("debug").ToLocalChecked()).ToLocalChecked()).ToLocalChecked();
        if (Nan::Has(debug, Nan::New("tileBorders").ToLocalChecked()).FromJust()) {
            if (Nan::Get(debug, Nan::New("tileBorders").ToLocalChecked()).ToLocalChecked()->BooleanValue()) {
                options.debugOptions = options.debugOptions | mbgl::MapDebugOptions::TileBorders;
            }
        }
        if (Nan::Has(debug, Nan::New("parseStatus").ToLocalChecked()).FromJust()) {
            if (Nan::Get(debug, Nan::New("parseStatus").ToLocalChecked()).ToLocalChecked()->BooleanValue()) {
                options.debugOptions = options.debugOptions | mbgl::MapDebugOptions::ParseStatus;
            }
        }
        if (Nan::Has(debug, Nan::New("timestamps").ToLocalChecked()).FromJust()) {
            if (Nan::Get(debug, Nan::New("timestamps").ToLocalChecked()).ToLocalChecked()->BooleanValue()) {
                options.debugOptions = options.debugOptions | mbgl::MapDebugOptions::Timestamps;
            }
        }
        if (Nan::Has(debug, Nan::New("collision").ToLocalChecked()).FromJust()) {
            if (Nan::Get(debug, Nan::New("collision").ToLocalChecked()).ToLocalChecked()->BooleanValue()) {
                options.debugOptions = options.debugOptions | mbgl::MapDebugOptions::Collision;
            }
        }
        if (Nan::Has(debug, Nan::New("overdraw").ToLocalChecked()).FromJust()) {
            if (Nan::Get(debug, Nan::New("overdraw").ToLocalChecked()).ToLocalChecked()->BooleanValue()) {
                options.debugOptions = mbgl::MapDebugOptions::Overdraw;
            }
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
 * @param {Array<string>} [options.classes=[]] style classes
 * @param {Function} callback
 * @returns {undefined} calls callback
 * @throws {Error} if stylesheet is not loaded or if map is already rendering
 */
void NodeMap::Render(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an options object");
    }

    if (info.Length() <= 1 || !info[1]->IsFunction()) {
        return Nan::ThrowTypeError("Second argument must be a callback function");
    }

    if (!nodeMap->loaded) {
        return Nan::ThrowTypeError("Style is not loaded");
    }

    if (nodeMap->callback) {
        return Nan::ThrowError("Map is currently rendering an image");
    }

    auto options = ParseOptions(Nan::To<v8::Object>(info[0]).ToLocalChecked());

    assert(!nodeMap->callback);
    assert(!nodeMap->image.data);
    nodeMap->callback = std::make_unique<Nan::Callback>(info[1].As<v8::Function>());

    try {
        nodeMap->startRender(std::move(options));
    } catch (mbgl::util::Exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::startRender(NodeMap::RenderOptions options) {
    map->setSize({ options.width, options.height });

    const mbgl::Size fbSize{ static_cast<uint32_t>(options.width * pixelRatio),
                             static_cast<uint32_t>(options.height * pixelRatio) };
    if (!view || view->getSize() != fbSize) {
        view.reset();
        mbgl::BackendScope scope { backend };
        view = std::make_unique<mbgl::OffscreenView>(backend.getContext(), fbSize);
    }

    if (map->getZoom() != options.zoom) {
        map->setZoom(options.zoom);
    }

    mbgl::LatLng latLng(options.latitude, options.longitude);
    if (map->getLatLng() != latLng) {
        map->setLatLng(latLng);
    }

    if (map->getBearing() != options.bearing) {
        map->setBearing(options.bearing);
    }

    if (map->getPitch() != options.pitch) {
        map->setPitch(options.pitch);
    }

    if (map->getDebug() != options.debugOptions) {
        map->setDebug(options.debugOptions);
    }

    map->renderStill(*view, [this](const std::exception_ptr eptr) {
        if (eptr) {
            error = std::move(eptr);
            uv_async_send(async);
        } else {
            assert(!image.data);
            image = view->readStillImage();
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
    assert(!image.data);

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
    } else if (img.data) {
        v8::Local<v8::Object> pixels = Nan::NewBuffer(
            reinterpret_cast<char *>(img.data.get()), img.bytes(),
            // Retain the data until the buffer is deleted.
            [](char *, void * hint) {
                delete [] reinterpret_cast<uint8_t*>(hint);
            },
            img.data.get()
        ).ToLocalChecked();
        img.data.release();

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
void NodeMap::Release(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    try {
        nodeMap->release();
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::release() {
    if (!map) throw mbgl::util::Exception(releasedMessage());

    uv_close(reinterpret_cast<uv_handle_t *>(async), [] (uv_handle_t *h) {
        delete reinterpret_cast<uv_async_t *>(h);
    });

    map.reset();
}

/**
 * Cancel an ongoing render request. The callback will be called with
 * the error set to "Canceled". Will throw if no rendering is in progress.
 * @name cancel
 * @returns {undefined}
 */
void NodeMap::Cancel(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());

    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());
    if (!nodeMap->callback) return Nan::ThrowError("No render in progress");

    try {
        nodeMap->cancel();
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::cancel() {
    auto style = map->getStyleJSON();

    map = std::make_unique<mbgl::Map>(backend, mbgl::Size{ 256, 256 },
            pixelRatio, *this, threadpool, mbgl::MapMode::Still);

    // FIXME: Reload the style after recreating the map. We need to find
    // a better way of canceling an ongoing rendering on the core level
    // without resetting the map, which is way too expensive.
    map->setStyleJSON(style);

    error = std::make_exception_ptr(std::runtime_error("Canceled"));
    renderFinished();
}

void NodeMap::AddSource(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() != 2) {
        return Nan::ThrowTypeError("Two argument required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    Error error;
    mbgl::optional<std::unique_ptr<Source>> source = convert<std::unique_ptr<Source>>(info[1], error, *Nan::Utf8String(info[0]));
    if (!source) {
        Nan::ThrowTypeError(error.message.c_str());
        return;
    }

    nodeMap->map->getStyle().addSource(std::move(*source));
}

void NodeMap::AddLayer(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() != 1) {
        return Nan::ThrowTypeError("One argument required");
    }

    Error error;
    mbgl::optional<std::unique_ptr<Layer>> layer = convert<std::unique_ptr<Layer>>(info[0], error);
    if (!layer) {
        Nan::ThrowTypeError(error.message.c_str());
        return;
    }

    nodeMap->map->getStyle().addLayer(std::move(*layer));
}

void NodeMap::RemoveLayer(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() != 1) {
        return Nan::ThrowTypeError("One argument required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    nodeMap->map->getStyle().removeLayer(*Nan::Utf8String(info[0]));
}

void NodeMap::AddImage(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() != 3) {
        return Nan::ThrowTypeError("Three arguments required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    if (!info[1]->IsObject()) {
        return Nan::ThrowTypeError("Second argument must be an object");
    }

    if (!info[2]->IsObject()) {
        return Nan::ThrowTypeError("Third argument must be an object");
    }

    auto optionObject = Nan::To<v8::Object>(info[2]).ToLocalChecked();

    if (!Nan::Get(optionObject, Nan::New("height").ToLocalChecked()).ToLocalChecked()->IsUint32()) {
        return Nan::ThrowTypeError("height parameter required");
    }

    if (!Nan::Get(optionObject, Nan::New("width").ToLocalChecked()).ToLocalChecked()->IsUint32()) {
        return Nan::ThrowTypeError("width parameter required");
    }

    if (!Nan::Get(optionObject, Nan::New("pixelRatio").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
        return Nan::ThrowTypeError("pixelRatio parameter required");
    }

    uint32_t imageHeight = Nan::Get(optionObject, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
    uint32_t imageWidth = Nan::Get(optionObject, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();

    if (imageWidth > 1024 || imageHeight > 1024) {
        return Nan::ThrowTypeError("Max height and width is 1024");
    }

    float pixelRatio = Nan::Get(optionObject, Nan::New("pixelRatio").ToLocalChecked()).ToLocalChecked()->NumberValue();
    auto imageBuffer = Nan::To<v8::Object>(info[1]).ToLocalChecked()->ToObject();
    
    char * imageDataBuffer = node::Buffer::Data(imageBuffer);
    size_t imageLength = node::Buffer::Length(imageBuffer);

    if (imageLength != imageHeight * imageWidth * 4) {
        return Nan::ThrowTypeError("Image size does not match buffer size");
    }

    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(imageLength);
    std::copy(imageDataBuffer, imageDataBuffer + imageLength, data.get());
    
    mbgl::UnassociatedImage cImage({ imageWidth, imageHeight}, std::move(data));
    mbgl::PremultipliedImage cPremultipliedImage = mbgl::util::premultiply(std::move(cImage));
    nodeMap->map->getStyle().addImage(std::make_unique<mbgl::style::Image>(*Nan::Utf8String(info[0]), std::move(cPremultipliedImage), pixelRatio));
}

void NodeMap::RemoveImage(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() != 1) {
        return Nan::ThrowTypeError("One argument required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    nodeMap->map->getStyle().removeImage(*Nan::Utf8String(info[0]));
}

void NodeMap::SetLayoutProperty(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() < 3) {
        return Nan::ThrowTypeError("Three arguments required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    mbgl::style::Layer* layer = nodeMap->map->getStyle().getLayer(*Nan::Utf8String(info[0]));
    if (!layer) {
        return Nan::ThrowTypeError("layer not found");
    }

    if (!info[1]->IsString()) {
        return Nan::ThrowTypeError("Second argument must be a string");
    }

    mbgl::optional<Error> error = setLayoutProperty(*layer, *Nan::Utf8String(info[1]), info[2]);
    if (error) {
        return Nan::ThrowTypeError(error->message.c_str());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::SetPaintProperty(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() < 3) {
        return Nan::ThrowTypeError("Three arguments required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    mbgl::style::Layer* layer = nodeMap->map->getStyle().getLayer(*Nan::Utf8String(info[0]));
    if (!layer) {
        return Nan::ThrowTypeError("layer not found");
    }

    if (!info[1]->IsString()) {
        return Nan::ThrowTypeError("Second argument must be a string");
    }

    mbgl::optional<Error> error = setPaintProperty(*layer, *Nan::Utf8String(info[1]), info[2]);
    if (error) {
        return Nan::ThrowTypeError(error->message.c_str());
    }

    info.GetReturnValue().SetUndefined();
}

struct SetFilterVisitor {
    mbgl::style::Filter& filter;

    void operator()(mbgl::style::CustomLayer&) {
        Nan::ThrowTypeError("layer doesn't support filters");
    }

    void operator()(mbgl::style::RasterLayer&) {
        Nan::ThrowTypeError("layer doesn't support filters");
    }

    void operator()(mbgl::style::BackgroundLayer&) {
        Nan::ThrowTypeError("layer doesn't support filters");
    }

    template <class VectorLayer>
    void operator()(VectorLayer& layer) {
        layer.setFilter(filter);
    }
};

void NodeMap::SetFilter(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() < 2) {
        return Nan::ThrowTypeError("Two arguments required");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    mbgl::style::Layer* layer = nodeMap->map->getStyle().getLayer(*Nan::Utf8String(info[0]));
    if (!layer) {
        return Nan::ThrowTypeError("layer not found");
    }

    Filter filter;

    if (!info[1]->IsNull() && !info[1]->IsUndefined()) {
        Error error;
        mbgl::optional<Filter> converted = convert<Filter>(info[1], error);
        if (!converted) {
            Nan::ThrowTypeError(error.message.c_str());
            return;
        }
        filter = std::move(*converted);
    }

    layer->accept(SetFilterVisitor { filter });
}

void NodeMap::SetCenter(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsArray()) {
        return Nan::ThrowTypeError("First argument must be an array");
    }

    auto center = info[0].As<v8::Array>();
    double latitude = 0;
    double longitude = 0;
    if (center->Length() > 0) { longitude = Nan::Get(center, 0).ToLocalChecked()->NumberValue(); }
    if (center->Length() > 1) { latitude = Nan::Get(center, 1).ToLocalChecked()->NumberValue(); }

    try {
        nodeMap->map->setLatLng(mbgl::LatLng { latitude, longitude });
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::SetZoom(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsNumber()) {
        return Nan::ThrowTypeError("First argument must be a number");
    }

    try {
        nodeMap->map->setZoom(info[0]->NumberValue());
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::SetBearing(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsNumber()) {
        return Nan::ThrowTypeError("First argument must be a number");
    }

    try {
        nodeMap->map->setBearing(info[0]->NumberValue());
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::SetPitch(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsNumber()) {
        return Nan::ThrowTypeError("First argument must be a number");
    }

    try {
        nodeMap->map->setPitch(info[0]->NumberValue());
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().SetUndefined();
}

void NodeMap::DumpDebugLogs(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    nodeMap->map->dumpDebugLogs();
    info.GetReturnValue().SetUndefined();
}

void NodeMap::QueryRenderedFeatures(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto nodeMap = Nan::ObjectWrap::Unwrap<NodeMap>(info.Holder());
    if (!nodeMap->map) return Nan::ThrowError(releasedMessage());

    if (info.Length() <= 0 || !info[0]->IsArray()) {
        return Nan::ThrowTypeError("First argument must be an array");
    }

    auto posOrBox = info[0].As<v8::Array>();
    if (posOrBox->Length() != 2) {
        return Nan::ThrowTypeError("First argument must have two components");
    }

    mbgl::RenderedQueryOptions queryOptions;
    if (!info[1]->IsNull() && !info[1]->IsUndefined()) {
        if (!info[1]->IsObject()) {
            return Nan::ThrowTypeError("options argument must be an object");
        }
        
        auto options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
        
        //Check if layers is set. If provided, it must be an array of strings
        if (Nan::Has(options, Nan::New("layers").ToLocalChecked()).FromJust()) {
            auto layersOption = Nan::Get(options, Nan::New("layers").ToLocalChecked()).ToLocalChecked();
            if (!layersOption->IsArray()) {
                return Nan::ThrowTypeError("Requires options.layers property to be an array");
            }
            auto layers = layersOption.As<v8::Array>();
            std::vector<std::string> layersVec;
            for (uint32_t i=0; i < layers->Length(); i++) {
                layersVec.emplace_back(*Nan::Utf8String(Nan::Get(layers,i).ToLocalChecked()));
            }
            queryOptions.layerIDs = layersVec;
        }
        
        //Check if filter is provided. If set it must be a valid Filter object
        if (Nan::Has(options, Nan::New("filter").ToLocalChecked()).FromJust()) {
            auto filterOption = Nan::Get(options, Nan::New("filter").ToLocalChecked()).ToLocalChecked();
            Error error;
            mbgl::optional<Filter> converted = convert<Filter>(filterOption, error);
            if (!converted) {
                return Nan::ThrowTypeError(error.message.c_str());
            }
            queryOptions.filter = std::move(*converted);
        }
    }

    try {
        std::vector<mbgl::Feature> optional;

        if (Nan::Get(posOrBox, 0).ToLocalChecked()->IsArray()) {

            auto pos0 = Nan::Get(posOrBox, 0).ToLocalChecked().As<v8::Array>();
            auto pos1 = Nan::Get(posOrBox, 1).ToLocalChecked().As<v8::Array>();

            optional = nodeMap->map->queryRenderedFeatures(mbgl::ScreenBox {
                {
                    Nan::Get(pos0, 0).ToLocalChecked()->NumberValue(),
                    Nan::Get(pos0, 1).ToLocalChecked()->NumberValue()
                }, {
                    Nan::Get(pos1, 0).ToLocalChecked()->NumberValue(),
                    Nan::Get(pos1, 1).ToLocalChecked()->NumberValue()
                }
            },  queryOptions);

        } else {
            optional = nodeMap->map->queryRenderedFeatures(mbgl::ScreenCoordinate {
                Nan::Get(posOrBox, 0).ToLocalChecked()->NumberValue(),
                Nan::Get(posOrBox, 1).ToLocalChecked()->NumberValue()
            }, queryOptions);
        }

        auto array = Nan::New<v8::Array>();
        for (unsigned int i = 0; i < optional.size(); i++) {
            array->Set(i, toJS(optional[i]));
        }
        info.GetReturnValue().Set(array);
    } catch (const std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }
}

NodeMap::NodeMap(v8::Local<v8::Object> options)
    : pixelRatio([&] {
          Nan::HandleScope scope;
          return Nan::Has(options, Nan::New("ratio").ToLocalChecked()).FromJust()
                     ? Nan::Get(options, Nan::New("ratio").ToLocalChecked())
                           .ToLocalChecked()
                           ->NumberValue()
                     : 1.0;
      }()),
      map(std::make_unique<mbgl::Map>(backend,
                                      mbgl::Size{ 256, 256 },
                                      pixelRatio,
                                      *this,
                                      threadpool,
                                      mbgl::MapMode::Still)),
      async(new uv_async_t) {

    async->data = this;
    uv_async_init(uv_default_loop(), async, [](uv_async_t* h) {
        reinterpret_cast<NodeMap *>(h->data)->renderFinished();
    });

    // Make sure the async handle doesn't keep the loop alive.
    uv_unref(reinterpret_cast<uv_handle_t *>(async));
}

NodeMap::~NodeMap() {
    if (map) release();
}

std::unique_ptr<mbgl::AsyncRequest> NodeMap::request(const mbgl::Resource& resource, mbgl::FileSource::Callback callback_) {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[] = {
        Nan::New<v8::External>(this),
        Nan::New<v8::External>(&callback_)
    };

    auto instance = Nan::New(NodeRequest::constructor)->NewInstance(2, argv);

    Nan::Set(instance, Nan::New("url").ToLocalChecked(), Nan::New(resource.url).ToLocalChecked());
    Nan::Set(instance, Nan::New("kind").ToLocalChecked(), Nan::New<v8::Integer>(resource.kind));

    auto request = Nan::ObjectWrap::Unwrap<NodeRequest>(instance);
    request->Execute();

    return std::make_unique<NodeRequest::NodeAsyncRequest>(request);
}

} // namespace node_mbgl
