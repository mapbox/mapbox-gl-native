#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <node.h>
#include <nan.h>
#pragma GCC diagnostic pop

#include <queue>

namespace node_mbgl {

class NodeFileSource;

class NodeMap : public node::ObjectWrap {
    struct RenderOptions;
    class RenderWorker;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);
    static NAN_METHOD(SetAccessToken);
    static NAN_METHOD(Load);
    static NAN_METHOD(Render);

    void startRender(std::unique_ptr<NodeMap::RenderOptions> options);
    void renderFinished();

    static std::unique_ptr<NodeMap::RenderOptions> ParseOptions(v8::Local<v8::Object> obj);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
private:
    NodeMap(v8::Handle<v8::Object> source);
    ~NodeMap();

private:
    // For retaining the FileSource object.
    v8::Persistent<v8::Object> source;

    mbgl::HeadlessView view;
    NodeFileSource &fs;
    mbgl::Map map;

    std::unique_ptr<const mbgl::StillImage> image;
    std::unique_ptr<NanCallback> callback;

    // Async for delivering the notifications of render completion.
    uv_async_t *async;
};

}
