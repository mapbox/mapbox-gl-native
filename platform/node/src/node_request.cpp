#include "node_request.hpp"
#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>
#include <iostream>

namespace node_mbgl {

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

Nan::Persistent<v8::Function> NodeRequest::constructor;

NAN_MODULE_INIT(NodeRequest::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Request").ToLocalChecked());

    constructor.Reset(tpl->GetFunction());
    Nan::Set(target, Nan::New("Request").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(NodeRequest::New) {
    auto req = new NodeRequest(*reinterpret_cast<mbgl::FileSource::Callback*>(info[0].As<v8::External>()->Value()));
    req->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

v8::Handle<v8::Object> NodeRequest::Create(const mbgl::Resource& resource, mbgl::FileSource::Callback callback) {
    Nan::EscapableHandleScope scope;

    v8::Local<v8::Value> argv[] = {
        Nan::New<v8::External>(const_cast<mbgl::FileSource::Callback*>(&callback))
    };
    auto instance = Nan::New(constructor)->NewInstance(1, argv);

    Nan::Set(instance, Nan::New("url").ToLocalChecked(), Nan::New(resource.url).ToLocalChecked());
    Nan::Set(instance, Nan::New("kind").ToLocalChecked(), Nan::New<v8::Integer>(int(resource.kind)));

    return scope.Escape(instance);
}

NAN_METHOD(NodeRequest::Respond) {
    using Error = mbgl::Response::Error;

    mbgl::Response response;

    if (info.Length() < 1) {
        response.noContent = true;

    } else if (info[0]->BooleanValue()) {
        std::unique_ptr<Nan::Utf8String> message;

        // Store the error string.
        if (info[0]->IsObject()) {
            auto err = info[0]->ToObject();
            if (Nan::Has(err, Nan::New("message").ToLocalChecked()).FromJust()) {
                message = std::make_unique<Nan::Utf8String>(
                    Nan::Get(err, Nan::New("message").ToLocalChecked())
                        .ToLocalChecked()
                        ->ToString());
            }
        }

        if (!message) {
            message = std::make_unique<Nan::Utf8String>(info[0]->ToString());
        }
        response.error = std::make_unique<Error>(
            Error::Reason::Other, std::string{ **message, size_t(message->length()) });

    } else if (info.Length() < 2 || !info[1]->IsObject()) {
        return Nan::ThrowTypeError("Second argument must be a response object");

    } else {
        auto res = info[1]->ToObject();

        if (Nan::Has(res, Nan::New("modified").ToLocalChecked()).FromJust()) {
            const double modified = Nan::Get(res, Nan::New("modified").ToLocalChecked()).ToLocalChecked()->ToNumber()->Value();
            if (!std::isnan(modified)) {
                response.modified = mbgl::SystemClock::from_time_t(modified / 1000);
            }
        }

        if (Nan::Has(res, Nan::New("expires").ToLocalChecked()).FromJust()) {
            const double expires = Nan::Get(res, Nan::New("expires").ToLocalChecked()).ToLocalChecked()->ToNumber()->Value();
            if (!std::isnan(expires)) {
                response.expires = mbgl::SystemClock::from_time_t(expires / 1000);
            }
        }

        if (Nan::Has(res, Nan::New("etag").ToLocalChecked()).FromJust()) {
            auto etagHandle = Nan::Get(res, Nan::New("etag").ToLocalChecked()).ToLocalChecked();
            if (etagHandle->BooleanValue()) {
                const Nan::Utf8String etag { etagHandle->ToString() };
                response.etag = std::string { *etag, size_t(etag.length()) };
            }
        }

        if (Nan::Has(res, Nan::New("data").ToLocalChecked()).FromJust()) {
            auto dataHandle = Nan::Get(res, Nan::New("data").ToLocalChecked()).ToLocalChecked();
            if (node::Buffer::HasInstance(dataHandle)) {
                response.data = std::make_shared<std::string>(
                    node::Buffer::Data(dataHandle),
                    node::Buffer::Length(dataHandle)
                );
            } else {
                return Nan::ThrowTypeError("Response data must be a Buffer");
            }
        }
    }

    // Send the response object to the NodeFileSource object
    Nan::ObjectWrap::Unwrap<NodeRequest>(info.Data().As<v8::Object>())->callback(response);
    info.GetReturnValue().SetUndefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

NodeRequest::NodeRequest(mbgl::FileSource::Callback callback_)
    : callback(callback_) {}

}
