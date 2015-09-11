#include "node_view.hpp"

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/util/exception.hpp>

#include <unistd.h>

namespace node_mbgl {

struct NodeView::Options {
    uint16_t width = 512;
    uint16_t height = 512;
    float ratio = 1.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

Nan::Persistent<v8::FunctionTemplate> NodeView::constructorTemplate;

static std::shared_ptr<mbgl::HeadlessDisplay> sharedDisplay() {
    static auto display = std::make_shared<mbgl::HeadlessDisplay>();
    return display;
}

NAN_MODULE_INIT(NodeView::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("View").ToLocalChecked());

    constructorTemplate.Reset(tpl);

    Nan::Set(target, Nan::New("View").ToLocalChecked(), tpl->GetFunction());

    // Initialize display connection on module load.
    sharedDisplay();
}

NAN_METHOD(NodeView::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new View objects");
    }

    if (info.Length() < 1 || !info[0]->IsObject()) {
        return Nan::ThrowTypeError("Requires an options object as first argument");
    }

    auto obj = info[0]->ToObject();
    auto options = new Options;

    if (Nan::Has(obj, Nan::New("width").ToLocalChecked()).FromJust()
     && Nan::Get(obj, Nan::New("width").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
        options->width = Nan::Get(obj, Nan::New("width").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    } else {
        return Nan::ThrowError("Options object must have a numerical 'width' property");
    }

    if (Nan::Has(obj, Nan::New("height").ToLocalChecked()).FromJust()
     && Nan::Get(obj, Nan::New("height").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
        options->height = Nan::Get(obj, Nan::New("height").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    } else {
        return Nan::ThrowError("Options object must have a numerical 'height' property");
    }

    if (Nan::Has(obj, Nan::New("ratio").ToLocalChecked()).FromJust()
     && Nan::Get(obj, Nan::New("ratio").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
        options->ratio = Nan::Get(obj, Nan::New("ratio").ToLocalChecked()).ToLocalChecked()->IntegerValue();
    } else {
        return Nan::ThrowError("Options object must have a numerical 'ratio' property");
    }

    try {
        auto nodeView = new NodeView(options);
        nodeView->Wrap(info.This());
    } catch(std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().Set(info.This());
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

NodeView::NodeView(Options* options)
  : view(std::make_unique<mbgl::HeadlessView>(sharedDisplay(), options->ratio, options->width, options->height)) {}

NodeView::~NodeView() {
    view.reset();
}

}
