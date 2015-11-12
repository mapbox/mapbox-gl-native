#include "node_file_source.hpp"
#include "node_request.hpp"
#include "util/async_queue.hpp"

#include <mbgl/storage/request.hpp>

namespace mbgl {

struct NodeFileSource::Action {
    const enum : bool { Add, Cancel } type;
    const Resource resource;
};

NodeFileSource::NodeFileSource(v8::Local<v8::Object> options_)
    : options(options_),
    queue(new Queue(uv_default_loop(), [this](Action &action) {
        if (action.type == Action::Add) {
            processAdd(action.resource);
        } else if (action.type == Action::Cancel) {
            processCancel(action.resource);
        }
    })) {
    // Make sure that the queue doesn't block the loop from exiting.
    queue->unref();
}

NodeFileSource::~NodeFileSource() {
    MBGL_VERIFY_THREAD(tid);

    queue->stop();
    queue = nullptr;

    options.Reset();
}

Request* NodeFileSource::request(const Resource& resource, uv_loop_t* loop, Callback callback) {
    assert(l);

    auto req = new Request(resource, loop, std::move(callback));

    std::lock_guard<std::mutex> lock(observersMutex);

    // Add this request as an observer so that it'll get notified when something about this
    // request changes.
    assert(observers.find(resource) == observers.end());
    observers[resource] = req;

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue.
    queue->send(Action{ Action::Add, resource });

    return req;
}

void NodeFileSource::cancel(Request* req) {
    assert(req);

    req->cancel();

    std::lock_guard<std::mutex> lock(observersMutex);

    auto it = observers.find(req->resource);
    if (it == observers.end()) {
        // Exit early if no observers are found.
        return;
    }

    observers.erase(it);

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue.
    queue->send(Action{ Action::Cancel, req->resource });

    // Send a message back to the requesting thread and notify it that this request has been
    // canceled and is now safe to be deleted.
    req->destruct();
}

void NodeFileSource::processAdd(const Resource& resource) {
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

void NodeFileSource::processCancel(const Resource& resource) {
    Nan::HandleScope scope;

    auto it = pending.find(resource);
    if (it != pending.end()) {
        auto requestHandle = Nan::New(it->second);
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

        // Set the resource handle in the request wrapper handle to null
        Nan::ObjectWrap::Unwrap<NodeRequest>(requestHandle)->cancel();
    } else {
        // There is no request for this URL anymore. Likely, the request already completed
        // before we got around to process the cancelation request.
    }
}

void NodeFileSource::notify(const Resource& resource, const std::shared_ptr<const Response>& response) {
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
        // Exit early if no observers are found.
        return;
    }

    observersIt->second->notify(response);
}

}
