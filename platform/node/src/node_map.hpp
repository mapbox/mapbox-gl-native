#pragma once

#include "node_thread_pool.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/image.hpp>

#include <exception>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

namespace mbgl {
class Map;
class HeadlessFrontend;
} // namespace mbgl

namespace node_mbgl {

class NodeMapObserver : public mbgl::MapObserver {
    void onDidFailLoadingMap(std::exception_ptr) override;
};

class RenderRequest;

class NodeMap : public Nan::ObjectWrap,
                public mbgl::FileSource {
public:
    struct RenderOptions;
    class RenderWorker;

    NodeMap(v8::Local<v8::Object>);
    ~NodeMap();

    static Nan::Persistent<v8::Function> constructor;
    static Nan::Persistent<v8::Object> parseError;

    static void Init(v8::Local<v8::Object>);

    static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Load(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Loaded(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Render(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Release(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Cancel(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void AddSource(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void RemoveSource(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void AddLayer(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void RemoveLayer(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void AddImage(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void RemoveImage(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetLayerZoomRange(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetLayoutProperty(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetPaintProperty(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetFilter(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetCenter(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetZoom(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetBearing(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetPitch(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetLight(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetAxonometric(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetXSkew(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void SetYSkew(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void DumpDebugLogs(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void QueryRenderedFeatures(const Nan::FunctionCallbackInfo<v8::Value>&);

    static v8::Local<v8::Value> ParseError(const char* msg);

    void startRender(RenderOptions options);
    void renderFinished();

    void release();
    void cancel();

    static RenderOptions ParseOptions(v8::Local<v8::Object>);

    std::unique_ptr<mbgl::AsyncRequest> request(const mbgl::Resource&, mbgl::FileSource::Callback);

    const float pixelRatio;
    mbgl::MapMode mode;
    NodeThreadPool threadpool;
    NodeMapObserver mapObserver;
    std::unique_ptr<mbgl::HeadlessFrontend> frontend;
    std::unique_ptr<mbgl::Map> map;

    std::exception_ptr error;
    mbgl::PremultipliedImage image;
    std::unique_ptr<RenderRequest> req;

    // Async for delivering the notifications of render completion.
    uv_async_t *async;

    bool loaded = false;
};

}
