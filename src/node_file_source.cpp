#include "node_file_source.hpp"
#include "node_request.hpp"
#include "util/async_queue.hpp"

#include <mbgl/storage/request.hpp>

#include <algorithm>

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
    mbgl::Resource const resource;
};

NodeFileSource::NodeFileSource() :
    queue(new Queue(uv_default_loop(), [this](Action &action) {
        if (action.type == Action::Add) {
            processAdd(action.resource);
        } else if (action.type == Action::Cancel) {
            processCancel(action.resource);
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
    auto req = new mbgl::Request(resource, loop, std::move(callback));

    std::lock_guard<std::mutex> lock(observersMutex);
    auto it = observers.find(resource);
    if (it == observers.end()) {
        observers[resource] = { req };

        // This function can be called from any thread. Make sure we're executing the actual call in the
        // file source loop by sending it over the queue. It will be processed in processAction().
        queue->send(Action{ Action::Add, resource });
    } else {
        it->second.emplace_back(req);
    }

    return req;
}

void NodeFileSource::cancel(mbgl::Request* req) {
    req->cancel();

    std::lock_guard<std::mutex> lock(observersMutex);

    auto observersIter = observers.find(req->resource);
    if (observersIter == observers.end()) {
        return;
    }

    auto& observersList = observersIter->second;
    auto observersListIter = std::find(observersList.begin(), observersList.end(), req);
    if (observersListIter == observersList.end()) {
        return;
    }

    observersList.erase(observersListIter);
    if (observersList.empty()) {
        observers.erase(observersIter);

        // This function can be called from any thread. Make sure we're executing the actual call in the
        // file source loop by sending it over the queue. It will be processed in processAction().
        queue->send(Action{ Action::Cancel, req->resource });
    }

    req->destruct();
}

void NodeFileSource::processAdd(const mbgl::Resource& resource) {
    NanScope();

    // Make sure the loop stays alive as long as request is pending.
    if (pending.empty()) {
        queue->ref();
    }

    auto handle = NanObjectWrapHandle(this);
    auto requestHandle = NanNew<v8::Object>(NodeRequest::Create(handle, resource));

    v8::Persistent<v8::Object> requestPersistent;
    NanAssignPersistent(requestPersistent, requestHandle);
    pending.emplace(resource, std::move(requestPersistent));

    v8::Local<v8::Value> argv[] = { requestHandle };
    NanMakeCallback(handle, NanNew("request"), 1, argv);
}

void NodeFileSource::processCancel(const mbgl::Resource& resource) {
    NanScope();

    auto it = pending.find(resource);
    if (it == pending.end()) {
        // The response callback was already fired. There is no point in calling the cancelation
        // callback because the request is already completed.
    } else {
#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
        auto requestHandle = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), it->second);
        it->second.Reset();
#else
        auto requestHandle = NanNew<v8::Object>(it->second);
        NanDisposePersistent(it->second);
#endif
        pending.erase(it);

        // Make sure the the loop can exit when there are no pending requests.
        if (pending.empty()) {
            queue->unref();
        }

        auto handle = NanObjectWrapHandle(this);
        if (handle->Has(NanNew("cancel"))) {
            v8::Local<v8::Value> argv[] = { requestHandle };
            NanMakeCallback(handle, NanNew("cancel"), 1, argv);
        }

        // Set the request handle in the request wrapper handle to null
        ObjectWrap::Unwrap<NodeRequest>(requestHandle)->cancel();
    }
}

void NodeFileSource::notify(const mbgl::Resource& resource, const std::shared_ptr<const mbgl::Response>& response) {
    // First, remove the request, since it might be destructed at any point now.
    auto it = pending.find(resource);
    if (it != pending.end()) {
#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
        it->second.Reset();
#else
        NanDisposePersistent(it->second);
#endif
        pending.erase(it);

        // Make sure the the loop can exit when there are no pending requests.
        if (pending.empty()) {
            queue->unref();
        }
    }

    std::vector<mbgl::Request*> observersList;

    {
        std::lock_guard<std::mutex> lock(observersMutex);

        auto observersIter = observers.find(resource);
        if (observersIter == observers.end()) {
            return;
        }

        observersList.swap(observersIter->second);
        observers.erase(observersIter);
    }

    for (auto request : observersList) {
        request->notify(response);
    }
}

}
