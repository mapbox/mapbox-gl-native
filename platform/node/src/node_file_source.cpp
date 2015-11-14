#include "node_file_source.hpp"
#include "node_request.hpp"
#include "node_mapbox_gl_native.hpp"

namespace node_mbgl {

class NodeFileSourceRequest : public mbgl::FileRequest {
public:
    std::unique_ptr<mbgl::WorkRequest> workRequest;
};

NodeFileSource::NodeFileSource(v8::Local<v8::Object> options_) {
    options.Reset(options_);
}

NodeFileSource::~NodeFileSource() {
    options.Reset();
}

std::unique_ptr<mbgl::FileRequest> NodeFileSource::request(const mbgl::Resource& resource, Callback callback) {
    auto req = std::make_unique<NodeFileSourceRequest>();

    // This function can be called from any thread. Make sure we're executing the
    // JS implementation in the node event loop.
    req->workRequest = NodeRunLoop().invokeWithCallback([this] (mbgl::Resource res, Callback cb) {
        Nan::HandleScope scope;

        auto requestHandle = NodeRequest::Create(res, cb)->ToObject();
        auto callbackHandle = Nan::GetFunction(Nan::New<v8::FunctionTemplate>(NodeRequest::Respond, requestHandle)).ToLocalChecked();

        v8::Local<v8::Value> argv[] = { requestHandle, callbackHandle };
        Nan::MakeCallback(Nan::New(options), "request", 2, argv);
    }, callback, resource);

    return std::move(req);
}

}
