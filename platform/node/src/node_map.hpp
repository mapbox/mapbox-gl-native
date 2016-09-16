#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

class NodeMap : public Nan::ObjectWrap, public mbgl::FileSource {
public:
    struct RenderOptions;
    class RenderWorker;

    NodeMap(v8::Local<v8::Object>);
    ~NodeMap();

    static Nan::Persistent<v8::Function> constructor;

    static void Init(v8::Local<v8::Object> exports);

    static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Load(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Loaded(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Render(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Release(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void AddClass(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void AddSource(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void AddLayer(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void RemoveLayer(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetLayoutProperty(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetPaintProperty(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetFilter(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void DumpDebugLogs(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void QueryRenderedFeatures(const Nan::FunctionCallbackInfo<v8::Value>&);

    void startRender(RenderOptions options);
    void renderFinished();

    void release();

    static RenderOptions ParseOptions(v8::Local<v8::Object>);

    std::unique_ptr<mbgl::AsyncRequest> request(const mbgl::Resource&, mbgl::FileSource::Callback);

    mbgl::HeadlessView view;
    std::unique_ptr<mbgl::Map> map;

    std::exception_ptr error;
    mbgl::PremultipliedImage image;
    std::unique_ptr<Nan::Callback> callback;

    // Async for delivering the notifications of render completion.
    uv_async_t* async;

    bool loaded = false;
};
}
