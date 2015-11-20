#include "node_file_source.hpp"
#include "node_request.hpp"
#include "node_mapbox_gl_native.hpp"

#include <iostream>

namespace mbgl {

class NodeFileSourceRequest : public FileRequest {
public:
    std::unique_ptr<WorkRequest> workRequest;
};

NodeFileSource::NodeFileSource(Nan::Persistent<v8::Object>* options_)
    : options(options_) {}

NodeFileSource::~NodeFileSource() {
    std::cout << "~NodeFileSource" << std::endl;
}

std::unique_ptr<FileRequest> NodeFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<NodeFileSourceRequest>();

    // This function can be called from any thread. Make sure we're executing the
    // JS implementation in the node event loop.
    req->workRequest = util::NodeRunLoop().invokeWithCallback([this] (Resource res, Callback cb) {
        Nan::HandleScope scope;

        auto requestHandle = NodeRequest::Create(res, cb)->ToObject();
        auto callbackHandle = Nan::GetFunction(Nan::New<v8::FunctionTemplate>(NodeRequest::Respond, requestHandle)).ToLocalChecked();

        v8::Local<v8::Value> argv[] = { requestHandle, callbackHandle };

        // TODO: This will segfault if NodeFileSource and the options
        // persistent have already been garbage collected.
        // This needs a safety check or to be canceled in the
        // NodeFileSource destructor.
        Nan::MakeCallback(Nan::New(*options), "request", 2, argv);
    }, callback, resource);

    return std::move(req);
}

}
