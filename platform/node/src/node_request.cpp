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

    auto request = new NodeRequest(target, *callback);

    request->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

void NodeRequest::HandleCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    auto request = Nan::ObjectWrap::Unwrap<NodeRequest>(info.Holder());

    // Move out of the object so callback() can only be fired once.
    auto callback = std::move(request->callback);
    if (!callback) {
        return info.GetReturnValue().SetUndefined();
    }

    mbgl::Response response;

    if (info.Length() >= 1 && info[0]->BooleanValue()) {
        // First parameter is an error argument.
        auto err = Nan::To<v8::Object>(info[0]).ToLocalChecked();

        // Extract the status code from the Error object, if it has one.
        mbgl::ResourceStatus status = mbgl::ResourceStatus::OtherError;
        if (Nan::Has(err, Nan::New("code").ToLocalChecked()).FromJust()) {
            status = static_cast<mbgl::ResourceStatus>(
                Nan::To<uint32_t>(Nan::Get(err, Nan::New("code").ToLocalChecked()).ToLocalChecked())
                    .FromMaybe(static_cast<uint32_t>(mbgl::ResourceStatus::OtherError)));
            // Validate the enum value.
            switch (status) {
                    case mbgl::ResourceStatus::NotFoundError:
                    case mbgl::ResourceStatus::ServerError:
                    case mbgl::ResourceStatus::ConnectionError:
                    case mbgl::ResourceStatus::RateLimitError:
                    case mbgl::ResourceStatus::OtherError:
                        // Valid error enums.
                        break;
                    default:
                        // Invalid error enums.
                        return Nan::ThrowTypeError("Invalid error code. Must be one of NotFound, server, Connection, RateLimit, or Other");
            }
        }

        // Extract the error message from the Error object.
        std::string message;
        if (Nan::Has(err, Nan::New("message").ToLocalChecked()).FromJust()) {
            Nan::Utf8String string(
                Nan::Get(err, Nan::New("message").ToLocalChecked()).ToLocalChecked());
            message.assign(*string, string.length());
        } else {
            Nan::Utf8String string(err);
            message.assign(*string, string.length());
        }

        response.error = std::make_unique<mbgl::Response::Error>(status, message);
    } else if (info.Length() >= 2) {
        // This is a successful response. It has an object as the second parameter that
        // may contain data.
        auto res = Nan::To<v8::Object>(info[1]).ToLocalChecked();

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
        } else {
            response.noContent = true;
        }
    } else {
        response.noContent = true;
    }

    if (request->ErrorMessage()) {
        response.error = std::make_unique<mbgl::Response::Error>(
            mbgl::ResourceStatus::OtherError,
            request->ErrorMessage()
        );
    }

    // Send the response object to the NodeFileSource object
    callback(response);
    info.GetReturnValue().SetUndefined();
}

void NodeRequest::Execute() {
    v8::Local<v8::Value> argv[] = { handle() };

    Nan::MakeCallback(Nan::To<v8::Object>(target->handle()->GetInternalField(1)).ToLocalChecked(), "request", 1, argv);
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
