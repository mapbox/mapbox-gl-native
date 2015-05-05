#include "node_file_source.hpp"
#include "node_request.hpp"
#include "util/async_queue.hpp"

#include <mbgl/storage/request.hpp>

namespace node_mbgl {

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

v8::Persistent<v8::FunctionTemplate> NodeFileSource::constructorTemplate;

void NodeFileSource::Init(v8::Handle<v8::Object> target) {
    NanScope();

    v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(New);

    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(NanNew("FileSource"));

    NanAssignPersistent(constructorTemplate, t);

    target->Set(NanNew("FileSource"), t->GetFunction());
}

NAN_METHOD(NodeFileSource::New) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowTypeError("Use the new operator to create new FileSource objects");
    }

    auto fs = new NodeFileSource();
    fs->Wrap(args.This());

    NanReturnValue(args.This());
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

struct NodeFileSource::Action {
    const enum : bool { Add, Cancel } type;
    mbgl::Request *const request;
};

NodeFileSource::NodeFileSource() :
    queue(new Queue(uv_default_loop(), [this](Action &action) {
        if (action.type == Action::Add) {
            processAdd(action.request);
        } else if (action.type == Action::Cancel) {
            processCancel(action.request);
        }
    }))
{
    // Make sure that the queue doesn't block the loop from exiting.
    queue->unref();
}

NodeFileSource::~NodeFileSource() {
    queue->stop();
    queue = nullptr;
}

mbgl::Request* NodeFileSource::request(const mbgl::Resource& resource, uv_loop_t* loop, Callback callback) {
    auto request = new mbgl::Request(resource, loop, std::move(callback));

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(Action{ Action::Add, request });
    return request;
}

void NodeFileSource::cancel(mbgl::Request* request) {
    request->cancel();

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(Action{ Action::Cancel, request });
}

void NodeFileSource::request(const mbgl::Resource& resource, Callback callback) {
    auto request = new mbgl::Request(resource, nullptr, std::move(callback));

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(Action{ Action::Add, request });
}

void NodeFileSource::processAdd(mbgl::Request *request) {
    NanScope();

    // Make sure the loop stays alive as long as request is pending.
    if (pending.empty()) {
        queue->ref();
    }

    auto requestHandle = v8::Local<v8::Object>::New(NodeRequest::Create(handle_, request));
    pending.emplace(request, std::move(v8::Persistent<v8::Object>::New(requestHandle)));

    v8::Local<v8::Value> argv[] = { requestHandle };
    NanMakeCallback(handle_, NanNew("request"), 1, argv);
}

void NodeFileSource::processCancel(mbgl::Request *request) {
    NanScope();

    auto it = pending.find(request);
    if (it == pending.end()) {
        // The response callback was already fired. There is no point in calling the cancelation
        // callback because the request is already completed.
    } else {
        auto requestHandle = v8::Local<v8::Object>::New(it->second);

        // Dispose and remove the persistent handle
        it->second.Dispose();
        pending.erase(it);

        // Make sure the the loop can exit when there are no pending requests.
        if (pending.empty()) {
            queue->unref();
        }

        if (handle_->Has(NanNew("cancel"))) {
            v8::Local<v8::Value> argv[] = { requestHandle };
            NanMakeCallback(handle_, NanNew("cancel"), 1, argv);
        }

        // Set the request handle in the request wrapper handle to null
        ObjectWrap::Unwrap<NodeRequest>(requestHandle)->cancel();
    }

    // Finally, destruct the request object
    request->destruct();
}

void NodeFileSource::notify(mbgl::Request *request, const std::shared_ptr<const mbgl::Response>& response) {
    // First, remove the request, since it might be destructed at any point now.
    auto it = pending.find(request);
    if (it != pending.end()) {
        it->second.Dispose();
        pending.erase(it);

        // Make sure the the loop can exit when there are no pending requests.
        if (pending.empty()) {
            queue->unref();
        }
    }

    request->notify(response);
}

}
