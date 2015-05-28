#include "node_request.hpp"
#include "node_file_source.hpp"
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/response.hpp>

#include <cmath>

namespace node_mbgl {

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

v8::Persistent<v8::FunctionTemplate> NodeRequest::constructorTemplate;

void NodeRequest::Init(v8::Handle<v8::Object> target) {
    NanScope();

    v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(New);

    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(NanNew("Request"));

    NODE_SET_PROTOTYPE_METHOD(t, "respond", Respond);

    NanAssignPersistent(constructorTemplate, t);

    target->Set(NanNew("Request"), t->GetFunction());
}

NAN_METHOD(NodeRequest::New) {
    NanScope();

    // Extract the pointer from the first argument
    if (args.Length() < 2 || !NanHasInstance(NodeFileSource::constructorTemplate, args[0]) || !args[1]->IsExternal()) {
        return NanThrowTypeError("Cannot create Request objects explicitly");
    }

    auto request = reinterpret_cast<mbgl::Request *>(args[1].As<v8::External>()->Value());
    auto req = new NodeRequest(args[0]->ToObject(), request);
    req->Wrap(args.This());

    NanReturnValue(args.This());
}

v8::Handle<v8::Object> NodeRequest::Create(v8::Handle<v8::Object> source, mbgl::Request *request) {
    NanEscapableScope();

    v8::Local<v8::Value> argv[] = { NanNew<v8::Object>(source), NanNew<v8::External>(request) };
    auto instance = NanNew<v8::FunctionTemplate>(constructorTemplate)->GetFunction()->NewInstance(2, argv);

    instance->ForceSet(NanNew("url"), NanNew(request->resource.url), v8::ReadOnly);
    instance->ForceSet(NanNew("kind"), NanNew<v8::Integer>(int(request->resource.kind)), v8::ReadOnly);

    return NanEscapeScope(instance);
}

NAN_METHOD(NodeRequest::Respond) {
    auto nodeRequest = ObjectWrap::Unwrap<NodeRequest>(args.Holder());
    if (!nodeRequest->request) {
        return NanThrowError("Request has already been responded to, or was canceled.");
    }

    auto source = ObjectWrap::Unwrap<NodeFileSource>(NanNew<v8::Object>(nodeRequest->source));

    auto request = nodeRequest->request;
    nodeRequest->request = nullptr;

    if (args.Length() < 1) {
        return NanThrowTypeError("First argument must be an error object");
    } else if (args[0]->BooleanValue()) {
        auto response = std::make_shared<mbgl::Response>();

        response->status = mbgl::Response::Error;

        // Store the error string.
        const NanUtf8String message { args[0]->ToString() };
        response->message = std::string { *message, size_t(message.length()) };

        source->notify(request, response);
    } else if (args.Length() < 2 || !args[1]->IsObject()) {
        return NanThrowTypeError("Second argument must be a response object");
    } else {
        auto response = std::make_shared<mbgl::Response>();
        auto res = args[1]->ToObject();

        response->status = mbgl::Response::Successful;

        if (res->Has(NanNew("modified"))) {
            const double modified = res->Get(NanNew("modified"))->ToNumber()->Value();
            if (!std::isnan(modified)) {
                response->modified = modified / 1000; // JS timestamps are milliseconds
            }
        }

        if (res->Has(NanNew("expires"))) {
            const double expires = res->Get(NanNew("expires"))->ToNumber()->Value();
            if (!std::isnan(expires)) {
                response->expires = expires / 1000; // JS timestamps are milliseconds
            }
        }

        if (res->Has(NanNew("etag"))) {
            auto etagHandle = res->Get(NanNew("etag"));
            if (etagHandle->BooleanValue()) {
                const NanUtf8String etag { etagHandle->ToString() };
                response->etag = std::string { *etag, size_t(etag.length()) };
            }
        }

        if (res->Has(NanNew("data"))) {
            auto dataHandle = res->Get(NanNew("data"));
            if (node::Buffer::HasInstance(dataHandle)) {
                response->data = std::string {
                    node::Buffer::Data(dataHandle),
                    node::Buffer::Length(dataHandle)
                };
            } else {
                return NanThrowTypeError("Response data must be a Buffer");
            }
        }

        // Send the response object to the NodeFileSource object
        source->notify(request, response);
    }

    NanReturnUndefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

NodeRequest::NodeRequest(v8::Local<v8::Object> source_, mbgl::Request *request_)
    : request(request_) {
    NanAssignPersistent(source, source_);
}

NodeRequest::~NodeRequest() {
    NanDisposePersistent(source);
}

void NodeRequest::cancel() {
    request = nullptr;
}

}
