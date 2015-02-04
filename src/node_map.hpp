#ifndef NODE_MBGL_NODE_MAP
#define NODE_MBGL_NODE_MAP

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#include <node.h>
#include <nan.h>

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
    static NAN_METHOD(Load);
    static NAN_METHOD(Render);

    static std::unique_ptr<NodeMap::RenderOptions> ParseOptions(v8::Local<v8::Object> obj);

    static v8::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
private:
    NodeMap(v8::Handle<v8::Object> source);
    ~NodeMap();

    void processNext();

private:
    v8::Persistent<v8::Object> source;
    v8::Persistent<v8::Object> self;

    mbgl::HeadlessView view;
    NodeFileSource &fs;
    mbgl::Map map;

    std::queue<std::unique_ptr<RenderWorker>> queue_;
};

} // end ns node_mbgl

#endif
