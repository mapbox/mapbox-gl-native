#include "node_request.hpp"
#include "node_map.hpp"
#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>

namespace node_mbgl {

NodeRequest::NodeRequest(
    NodeMap* target_,
    mbgl::FileSource::Callback callback_)
    : AsyncWorker(nullptr),
    target(target_),
    callback(std::move(callback_)) {
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

NAN_MODULE_INIT(NodeRequest::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Request").ToLocalChecked());

    constructor.Reset(tpl->GetFunction());

    // TODO: Remove this from the public JavaScript API
    Nan::Set(target, Nan::New("Request").ToLocalChecked(), tpl->GetFunction());
}

void NodeRequest::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto target = reinterpret_cast<NodeMap*>(info[0].As<v8::External>()->Value());
    auto callback = reinterpret_cast<mbgl::FileSource::Callback*>(info[1].As<v8::External>()->Value());

    auto request = new NodeRequest(target, *callback);

    request->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

void NodeRequest::HandleCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // Move out of the object so callback() can only be fired once.
    auto request = Nan::ObjectWrap::Unwrap<NodeRequest>(info.Data().As<v8::Object>());
    auto callback = std::move(request->callback);
    if (!callback) {
        return info.GetReturnValue().SetUndefined();
    }

    mbgl::Response response;

    if (info.Length() < 1) {
        response.noContent = true;
    } else if (info[0]->IsObject()) {
        auto err = Nan::To<v8::Object>(info[0]).ToLocalChecked();
        auto msg = Nan::New("message").ToLocalChecked();

        if (Nan::Has(err, msg).FromJust()) {
            request->SetErrorMessage(*Nan::Utf8String(
                Nan::Get(err, msg).ToLocalChecked()));
        }
    } else if (info[0]->IsString()) {
        request->SetErrorMessage(*Nan::Utf8String(info[0]));
    } else if (info.Length() < 2 || !info[1]->IsObject()) {
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
                return Nan::ThrowTypeError("Response data must be a Buffer");
            }
        }
    }

    if (request->ErrorMessage()) {
        response.error = std::make_unique<mbgl::Response::Error>(
            mbgl::Response::Error::Reason::Other,
            request->ErrorMessage()
        );
    }

    // Send the response object to the NodeFileSource object
    callback(response);
    info.GetReturnValue().SetUndefined();
}

void NodeRequest::Execute() {
    // Enter a new v8::Context to avoid leaking v8::FunctionTemplate
    // from Nan::New<v8::Function>
    v8::Local<v8::Context> context = v8::Context::New(v8::Isolate::GetCurrent());
    v8::Context::Scope scope(context);

    v8::Local<v8::Value> argv[] = { handle(), Nan::New<v8::Function>(NodeRequest::HandleCallback, handle()) };

    Nan::MakeCallback(Nan::To<v8::Object>(target->handle()->GetInternalField(1)).ToLocalChecked(), "request", 2, argv);
}

NodeRequest::NodeAsyncRequest::NodeAsyncRequest(NodeRequest* request_) : request(request_) {
    assert(request);

    // Make sure the JS object has a pointer to this so that it can remove
    // its pointer in the destructor
    request->asyncRequest = this;
}

NodeRequest::NodeAsyncRequest::~NodeAsyncRequest() {
    if (request) {
        // Remove the callback function because the AsyncRequest was
        // canceled and we are no longer interested in the result.
        request->callback = {};
        request->asyncRequest = nullptr;
    }
}

} // namespace node_mbgl
