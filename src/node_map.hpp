#pragma once

#include "node_file_source.hpp"

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

class NodeMap : public node::ObjectWrap {
    struct RenderOptions;
    class RenderWorker;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);
    static NAN_METHOD(Load);
    static NAN_METHOD(Render);
    static NAN_METHOD(Release);

    void startRender(std::unique_ptr<NodeMap::RenderOptions> options);
    void renderFinished();

    void release();

    inline bool isLoaded() { return loaded; }
    inline bool isValid() { return valid; }

    static std::unique_ptr<NodeMap::RenderOptions> ParseOptions(v8::Local<v8::Object> obj);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
private:
    NodeMap(v8::Handle<v8::Object>);
    ~NodeMap();

private:
    mbgl::HeadlessView view;
    NodeFileSource fs;
    std::unique_ptr<mbgl::Map> map;

    std::exception_ptr error;
    std::unique_ptr<const mbgl::StillImage> image;
    std::unique_ptr<NanCallback> callback;

    // Async for delivering the notifications of render completion.
    uv_async_t *async;

    bool loaded = false;
    bool valid = true;
};

}
