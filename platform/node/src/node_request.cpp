#include "node_request.hpp"
#include "node_map.hpp"
#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>

namespace node_mbgl {

NodeRequest::NodeRequest(
    mbgl::FileSource::Callback callback_,
    NodeAsyncRequest* asyncRequest_)
    : callback(std::move(callback_)),
    asyncRequest(asyncRequest_) {
        asyncRequest->request = this;
}

NodeRequest::~NodeRequest() {
    // When this object gets garbage collected, make sure that the
    // AsyncRequest can no longer attempt to remove the callback function
    // this object was holding (it can't be fired anymore).
    if (asyncRequest) {
        asyncRequest->request = nullptr;
    }
}

Nan::Persistent<v8::Function> NodeRequest::constructor;

void NodeRequest::Init() {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Request").ToLocalChecked());

    Nan::SetPrototypeMethod(tpl, "respond", HandleCallback);

    constructor.Reset(tpl->GetFunction());
}

void NodeRequest::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto target = reinterpret_cast<NodeMap*>(info[0].As<v8::External>()->Value());
    auto callback = reinterpret_cast<mbgl::FileSource::Callback*>(info[1].As<v8::External>()->Value());
    auto asyncRequest = reinterpret_cast<NodeAsyncRequest*>(info[2].As<v8::External>()->Value());

    auto request = new NodeRequest(*callback, asyncRequest);

    request->Wrap(info.This());
    request->Ref();
    Nan::Set(info.This(), Nan::New("url").ToLocalChecked(), info[3]);
    Nan::Set(info.This(), Nan::New("kind").ToLocalChecked(), info[4]);
    v8::Local<v8::Value> argv[] = { info.This() };
    request->asyncResource->runInAsyncScope(Nan::To<v8::Object>(target->handle()->GetInternalField(1)).ToLocalChecked(), "request", 1, argv);
    info.GetReturnValue().Set(info.This());
}

void NodeRequest::HandleCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto request = Nan::ObjectWrap::Unwrap<NodeRequest>(info.Holder());

    // Move out of the object so callback() can only be fired once.
    auto callback = std::move(request->callback);
    request->callback = {};
    if (!callback) {
        request->unref();
        return info.GetReturnValue().SetUndefined();
    }

    mbgl::Response response;

    if (info.Length() < 1) {
        response.noContent = true;
    } else if (info[0]->IsObject()) {
        auto err = Nan::To<v8::Object>(info[0]).ToLocalChecked();
        auto msg = Nan::New("message").ToLocalChecked();

        if (Nan::Has(err, msg).FromJust()) {
            response.error = std::make_unique<mbgl::Response::Error>(
                mbgl::Response::Error::Reason::Other,
                *Nan::Utf8String(Nan::Get(err, msg).ToLocalChecked())
            );
        }
    } else if (info[0]->IsString()) {
        response.error = std::make_unique<mbgl::Response::Error>(
            mbgl::Response::Error::Reason::Other,
            *Nan::Utf8String(info[0])
        );
    } else if (info.Length() < 2 || !info[1]->IsObject()) {
        request->unref();
        return Nan::ThrowTypeError("Second argument must be a response object");
    } else {
        auto res = Nan::To<v8::Object>(info[1]).ToLocalChecked();

        if (Nan::Has(res, Nan::New("modified").ToLocalChecked()).FromJust()) {
            const double modified = Nan::To<double>(Nan::Get(res, Nan::New("modified").ToLocalChecked()).ToLocalChecked()).FromJust();
            if (!std::isnan(modified)) {
                response.modified = mbgl::Timestamp{ mbgl::Seconds(
                    static_cast<mbgl::Seconds::rep>(modified / 1000)) };
            }
        }

        if (Nan::Has(res, Nan::New("expires").ToLocalChecked()).FromJust()) {
            const double expires = Nan::To<double>(Nan::Get(res, Nan::New("expires").ToLocalChecked()).ToLocalChecked()).FromJust();
            if (!std::isnan(expires)) {
                response.expires = mbgl::Timestamp{ mbgl::Seconds(
                    static_cast<mbgl::Seconds::rep>(expires / 1000)) };
            }
        }

        if (Nan::Has(res, Nan::New("etag").ToLocalChecked()).FromJust()) {
            const Nan::Utf8String etag(Nan::Get(res, Nan::New("etag").ToLocalChecked()).ToLocalChecked());
            response.etag = std::string { *etag, size_t(etag.length()) };
        }

        if (Nan::Has(res, Nan::New("data").ToLocalChecked()).FromJust()) {
            auto data = Nan::Get(res, Nan::New("data").ToLocalChecked()).ToLocalChecked();
            if (node::Buffer::HasInstance(data)) {
                response.data = std::make_shared<std::string>(
                    node::Buffer::Data(data),
                    node::Buffer::Length(data)
                );
            } else {
                request->unref();
                return Nan::ThrowTypeError("Response data must be a Buffer");
            }
        }
    }

    // Send the response object to the NodeFileSource object
    callback(response);
    request->unref();
    info.GetReturnValue().SetUndefined();
}

void NodeRequest::unref() {
  Nan::HandleScope scope;
  delete asyncResource;
  asyncResource = nullptr;
  Unref();
}

NodeAsyncRequest::NodeAsyncRequest() : request(nullptr) {}

NodeAsyncRequest::~NodeAsyncRequest() {
    if (request) {
        // Remove the callback function because the AsyncRequest was
        // canceled and we are no longer interested in the result.
        if (request->callback) {
            request->callback = {};
        }
        request->asyncRequest = nullptr;
    }
}

} // namespace node_mbgl
