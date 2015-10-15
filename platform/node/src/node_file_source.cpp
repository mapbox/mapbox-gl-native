#include "node_file_source.hpp"
#include "node_request.hpp"
#include "util/async_queue.hpp"

#include <mbgl/storage/request.hpp>

namespace node_mbgl {

struct NodeFileSource::Action {
    const enum : bool { Add, Cancel } type;
    mbgl::Resource const resource;
};

NodeFileSource::NodeFileSource(v8::Local<v8::Object> options_) :
    queue(new Queue(uv_default_loop(), [this](Action &action) {
        if (action.type == Action::Add) {
            processAdd(action.resource);
        } else if (action.type == Action::Cancel) {
            processCancel(action.resource);
        }
    }))
{
    options.Reset(options_);

    // Make sure that the queue doesn't block the loop from exiting.
    queue->unref();
}

NodeFileSource::~NodeFileSource() {
    queue->stop();
    queue = nullptr;

    options.Reset();
}

mbgl::Request* NodeFileSource::request(const mbgl::Resource& resource, uv_loop_t* loop, Callback callback) {
    auto req = new mbgl::Request(resource, loop, std::move(callback));

    std::lock_guard<std::mutex> lock(observersMutex);

    assert(observers.find(resource) == observers.end());
    observers[resource] = req;

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(Action{ Action::Add, resource });

    return req;
}

void NodeFileSource::cancel(mbgl::Request* req) {
    req->cancel();

    std::lock_guard<std::mutex> lock(observersMutex);

    auto it = observers.find(req->resource);
    if (it == observers.end()) {
        return;
    }

    observers.erase(it);

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(Action{ Action::Cancel, req->resource });

    req->destruct();
}

void NodeFileSource::processAdd(const mbgl::Resource& resource) {
    Nan::HandleScope scope;

    // Make sure the loop stays alive as long as request is pending.
    if (pending.empty()) {
        queue->ref();
    }

    auto requestHandle = NodeRequest::Create(this, resource)->ToObject();
    pending.emplace(resource, requestHandle);

    auto callback = Nan::GetFunction(Nan::New<v8::FunctionTemplate>(NodeRequest::Respond, requestHandle)).ToLocalChecked();
    callback->SetName(Nan::New("respond").ToLocalChecked());

    v8::Local<v8::Value> argv[] = { requestHandle, callback };
    Nan::MakeCallback(Nan::New(options), "request", 2, argv);
}

void NodeFileSource::processCancel(const mbgl::Resource& resource) {
    Nan::HandleScope scope;

    auto it = pending.find(resource);
    if (it == pending.end()) {
        // The response callback was already fired. There is no point in calling the cancelation
        // callback because the request is already completed.
    } else {
        v8::Local<v8::Object> requestHandle = Nan::New(it->second);
        it->second.Reset();
        pending.erase(it);

        // Make sure the the loop can exit when there are no pending requests.
        if (pending.empty()) {
            queue->unref();
        }

        if (Nan::Has(Nan::New(options), Nan::New("cancel").ToLocalChecked()).FromJust()) {
            v8::Local<v8::Value> argv[] = { requestHandle };
            Nan::MakeCallback(Nan::New(options), "cancel", 1, argv);
        }

        // Set the request handle in the request wrapper handle to null
        Nan::ObjectWrap::Unwrap<NodeRequest>(requestHandle)->cancel();
    }
}

void NodeFileSource::notify(const mbgl::Resource& resource, const std::shared_ptr<const mbgl::Response>& response) {
    // First, remove the request, since it might be destructed at any point now.
    auto it = pending.find(resource);
    if (it != pending.end()) {
        it->second.Reset();
        pending.erase(it);

        // Make sure the the loop can exit when there are no pending requests.
        if (pending.empty()) {
            queue->unref();
        }
    }

    std::lock_guard<std::mutex> lock(observersMutex);

    auto observersIt = observers.find(resource);
    if (observersIt == observers.end()) {
        return;
    }

    observersIt->second->notify(response);
}

}
