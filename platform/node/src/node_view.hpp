#pragma once

#include <mbgl/platform/default/headless_view.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <node.h>
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

class NodeView : public Nan::ObjectWrap {

struct Options;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Static Node Methods
public:
    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);

    static Nan::Persistent<v8::FunctionTemplate> constructorTemplate;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Instance
private:
    NodeView(Options*);
    ~NodeView();

    std::unique_ptr<mbgl::HeadlessView> view;

public:
    inline std::array<uint16_t, 2> getSize() { return view->getSize(); }
    inline float getPixelRatio() { return view->getPixelRatio(); }
    
    inline mbgl::HeadlessView* get() {
        return view.get();
    }

};

}
