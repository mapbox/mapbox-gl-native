#include "node_request.hpp"
#include "node_file_source.hpp"
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/response.hpp>

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
    // Extract the pointer from the first argument
    if (info.Length() < 2 || !info[0]->IsExternal() || !info[1]->IsExternal()) {
        return Nan::ThrowTypeError("Cannot create Request objects explicitly");
    }

    auto source = reinterpret_cast<NodeFileSource*>(info[0].As<v8::External>()->Value());
    auto resource = reinterpret_cast<mbgl::Resource*>(info[1].As<v8::External>()->Value());
    auto req = new NodeRequest(source, *resource);
    req->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

v8::Handle<v8::Object> NodeRequest::Create(NodeFileSource* source, const mbgl::Resource& resource) {
    Nan::EscapableHandleScope scope;

    v8::Local<v8::Value> argv[] = {
        Nan::New<v8::External>(const_cast<NodeFileSource*>(source)),
        Nan::New<v8::External>(const_cast<mbgl::Resource*>(&resource))
    };
    auto instance = Nan::New(constructor)->NewInstance(2, argv);

    Nan::Set(instance, Nan::New("url").ToLocalChecked(), Nan::New(resource.url).ToLocalChecked());
    Nan::Set(instance, Nan::New("kind").ToLocalChecked(), Nan::New<v8::Integer>(int(resource.kind)));

    return scope.Escape(instance);
}

NAN_METHOD(NodeRequest::Respond) {
    auto nodeRequest = Nan::ObjectWrap::Unwrap<NodeRequest>(info.Data().As<v8::Object>());

    // Request has already been responded to, or was canceled, fail silently.
    if (!nodeRequest->resource) {
        return info.GetReturnValue().SetUndefined();
    }

    auto source = nodeRequest->source;
    auto resource = std::move(nodeRequest->resource);

    if (info.Length() < 1) {
        auto response = std::make_shared<mbgl::Response>();
        response->status = mbgl::Response::NotFound;
        source->notify(*resource, response);
    } else if (info[0]->BooleanValue()) {
        auto response = std::make_shared<mbgl::Response>();

        response->status = mbgl::Response::Error;

        // Store the error string.
        const Nan::Utf8String message { info[0]->ToString() };
        response->message = std::string { *message, size_t(message.length()) };

        source->notify(*resource, response);
    } else if (info.Length() < 2 || !info[1]->IsObject()) {
        return Nan::ThrowTypeError("Second argument must be a response object");
    } else {
        auto response = std::make_shared<mbgl::Response>();
        auto res = info[1]->ToObject();

        response->status = mbgl::Response::Successful;

        if (Nan::Has(res, Nan::New("modified").ToLocalChecked()).FromJust()) {
            const double modified = Nan::Get(res, Nan::New("modified").ToLocalChecked()).ToLocalChecked()->ToNumber()->Value();
            if (!std::isnan(modified)) {
                response->modified = modified / 1000; // JS timestamps are milliseconds
            }
        }

        if (Nan::Has(res, Nan::New("expires").ToLocalChecked()).FromJust()) {
            const double expires = Nan::Get(res, Nan::New("expires").ToLocalChecked()).ToLocalChecked()->ToNumber()->Value();
            if (!std::isnan(expires)) {
                response->expires = expires / 1000; // JS timestamps are milliseconds
            }
        }

        if (Nan::Has(res, Nan::New("etag").ToLocalChecked()).FromJust()) {
            auto etagHandle = Nan::Get(res, Nan::New("etag").ToLocalChecked()).ToLocalChecked();
            if (etagHandle->BooleanValue()) {
                const Nan::Utf8String etag { etagHandle->ToString() };
                response->etag = std::string { *etag, size_t(etag.length()) };
            }
        }

        if (Nan::Has(res, Nan::New("data").ToLocalChecked()).FromJust()) {
            auto dataHandle = Nan::Get(res, Nan::New("data").ToLocalChecked()).ToLocalChecked();
            if (node::Buffer::HasInstance(dataHandle)) {
                response->data = std::string {
                    node::Buffer::Data(dataHandle),
                    node::Buffer::Length(dataHandle)
                };
            } else {
                return Nan::ThrowTypeError("Response data must be a Buffer");
            }
        }

        // Send the response object to the NodeFileSource object
        source->notify(*resource, response);
    }

    info.GetReturnValue().SetUndefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

NodeRequest::NodeRequest(NodeFileSource* source_, const mbgl::Resource& resource_)
    : source(source_),
    resource(std::make_unique<mbgl::Resource>(resource_)) {}

NodeRequest::~NodeRequest() {
}

void NodeRequest::cancel() {
    resource.reset();
}

}
