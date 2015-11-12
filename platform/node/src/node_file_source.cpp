#include "node_file_source.hpp"
#include "node_request.hpp"

namespace node_mbgl {

class NodeFileSourceRequest : public mbgl::FileRequest {
public:
    std::unique_ptr<mbgl::WorkRequest> workRequest;
};

NodeFileSource::NodeFileSource(v8::Local<v8::Object> options_)
    : nodeLoop(uv_default_loop()) {
    options.Reset(options_);

    // This has the effect of unreffing an async handle, which otherwise would keep the
    // default loop running. You would think we could do this in the destructor instead,
    // but in fact the destructor might not get called if there's no GC pressure which
    // would cause the NodeMap object which owns us to get destroyed.
    nodeLoop.stop();
}

NodeFileSource::~NodeFileSource() {
    options.Reset();
}

std::unique_ptr<mbgl::FileRequest> NodeFileSource::request(const mbgl::Resource& resource, Callback callback) {
    auto req = std::make_unique<NodeFileSourceRequest>();

    // This function can be called from any thread. Make sure we're executing the
    // JS implementation in the node event loop.
    req->workRequest = nodeLoop.invokeWithCallback([this] (mbgl::Resource res, Callback cb) {
        Nan::HandleScope scope;

        auto requestHandle = NodeRequest::Create(res, cb)->ToObject();
        auto callbackHandle = Nan::GetFunction(Nan::New<v8::FunctionTemplate>(NodeRequest::Respond, requestHandle)).ToLocalChecked();

        v8::Local<v8::Value> argv[] = { requestHandle, callbackHandle };
        Nan::MakeCallback(Nan::New(options), "request", 2, argv);
    }, callback, resource);

    return std::move(req);
}

}
