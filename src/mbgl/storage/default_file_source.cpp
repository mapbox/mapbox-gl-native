#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/default/request.hpp>
#include <mbgl/storage/default/asset_request.hpp>
#include <mbgl/storage/default/http_request.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/platform.hpp>

#include <mbgl/util/async_queue.hpp>
#include <mbgl/util/util.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/platform/log.hpp>

#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <thread>
#include <algorithm>
#include <cassert>


namespace algo = boost::algorithm;

namespace mbgl {

struct DefaultFileSource::ActionDispatcher {
    DefaultFileSource &fileSource;
    template <typename T> void operator()(T &t) { fileSource.process(t); }
};

struct DefaultFileSource::AddRequestAction {
    Request *const request;
};

struct DefaultFileSource::RemoveRequestAction {
    Request *const request;
};

struct DefaultFileSource::ResultAction {
    const Resource resource;
    std::unique_ptr<Response> response;
};

struct DefaultFileSource::StopAction {
};


DefaultFileSource::DefaultFileSource(FileCache *cache_, const std::string &root)
    : assetRoot(root.empty() ? platform::assetRoot() : root),
      loop(uv_loop_new()),
      cache(cache_),
      queue(new Queue(loop, [this](Action &action) {
          mapbox::util::apply_visitor(ActionDispatcher{*this}, action);
      })),
      thread([this]() {
#ifdef __APPLE__
          pthread_setname_np("FileSource");
#endif
          uv_run(loop, UV_RUN_DEFAULT);
      }) {
}

DefaultFileSource::DefaultFileSource(FileCache *cache_, uv_loop_t *loop_, const std::string &root)
    : assetRoot(root.empty() ? platform::assetRoot() : root),
      loop(loop_),
      cache(cache_),
      queue(new Queue(loop, [this](Action &action) {
          mapbox::util::apply_visitor(ActionDispatcher{*this}, action);
      })) {
    // Make sure that the queue doesn't block the loop from exiting.
    queue->unref();
}

DefaultFileSource::~DefaultFileSource() {
    MBGL_VERIFY_THREAD(tid);

    if (thread.joinable()) {
        if (queue) {
            queue->send(StopAction{ });
        }
        thread.join();
        uv_loop_delete(loop);
    } else {
        // Assume that the loop we received is running in the current thread.
        StopAction action {};
        process(action);
    }
}

SharedRequestBase *DefaultFileSource::find(const Resource &resource) {
    // We're using a set of pointers here instead of a map between url and SharedRequestBase because
    // we need to find the requests both by pointer and by URL. Given that the number of requests
    // is generally very small (typically < 10 at a time), hashing by URL incurs too much overhead
    // anyway.
    const auto it = pending.find(resource);
    if (it != pending.end()) {
        return it->second;
    }
    return nullptr;
}

Request *DefaultFileSource::request(const Resource &resource, uv_loop_t *l, Callback callback) {
    auto req = new Request(resource, l, std::move(callback));

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(AddRequestAction{ req });
    return req;
}

void DefaultFileSource::request(const Resource &resource, Callback callback) {
    auto req = new Request(resource, nullptr, std::move(callback));

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(AddRequestAction{ req });
}

void DefaultFileSource::cancel(Request *req) {
    req->cancel();

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue. It will be processed in processAction().
    queue->send(RemoveRequestAction{ req });
}

void DefaultFileSource::process(AddRequestAction &action) {
    const Resource &resource = action.request->resource;

    // We're adding a new Request.
    SharedRequestBase *sharedRequest = find(resource);
    if (!sharedRequest) {
        // There is no request for this URL yet. Create a new one and start it.
        if (algo::starts_with(resource.url, "asset://")) {
            sharedRequest = new AssetRequest(this, resource);
        } else {
            sharedRequest = new HTTPRequest(this, resource);
        }

        // Make sure the loop stays alive when we're not running the file source in it's own thread.
        if (!thread.joinable() && pending.empty()) {
            queue->ref();
        }

        const bool inserted = pending.emplace(resource, sharedRequest).second;
        assert(inserted);
        (void (inserted)); // silence unused variable warning on Release builds.

        // But first, we're going to start querying the database if it exists.
        if (!cache) {
            sharedRequest->start(loop);
        } else {
            // Otherwise, first check the cache for existing data so that we can potentially
            // revalidate the information without having to redownload everything.
            cache->get(resource, [this, resource](std::unique_ptr<Response> response) {
                queue->send(ResultAction { resource, std::move(response) });
            });
        }
    }
    sharedRequest->subscribe(action.request);
}

void DefaultFileSource::process(RemoveRequestAction &action) {
    SharedRequestBase *sharedRequest = find(action.request->resource);
    if (sharedRequest) {
        // If the number of dependent requests of the SharedRequestBase drops to zero, the
        // unsubscribe callback triggers the removal of the SharedRequestBase pointer from the list
        // of pending requests and initiates cancelation.
        sharedRequest->unsubscribe(action.request);
    } else {
        // There is no request for this URL anymore. Likely, the request already completed
        // before we got around to process the cancelation request.
    }

    // Send a message back to the requesting thread and notify it that this request has been
    // canceled and is now safe to be deleted.
    action.request->destruct();
}

void DefaultFileSource::process(ResultAction &action) {
    SharedRequestBase *sharedRequest = find(action.resource);
    if (sharedRequest) {
        if (action.response) {
            // This entry was stored in the cache. Now determine if we need to revalidate.
            const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count();
            if (action.response->expires > now) {
                // The response is fresh. We're good to notify the caller.
                sharedRequest->notify(std::move(action.response), FileCache::Hint::No);
                sharedRequest->cancel();
                return;
            } else {
                // The cached response is stale. Now run the real request.
                sharedRequest->start(loop, std::move(action.response));
            }
        } else {
            // There is no response. Now run the real request.
            sharedRequest->start(loop);
        }
    } else {
        // There is no request for this URL anymore. Likely, the request was canceled
        // before we got around to process the cache result.
    }
}

void DefaultFileSource::process(StopAction &) {
    // Cancel all remaining requests.
    for (auto it : pending) {
        it.second->unsubscribeAll();
    }
    pending.clear();

    assert(queue);
    queue->stop();
    queue = nullptr;
}

void DefaultFileSource::notify(SharedRequestBase *sharedRequest,
                               const std::set<Request *> &observers,
                               std::shared_ptr<const Response> response, FileCache::Hint hint) {
    // First, remove the request, since it might be destructed at any point now.
    assert(find(sharedRequest->resource) == sharedRequest);
    pending.erase(sharedRequest->resource);

    if (response) {
        if (cache) {
            // Store response in database
            cache->put(sharedRequest->resource, response, hint);
        }

        // Notify all observers.
        for (auto it : observers) {
            it->notify(response);
        }
    }

    if (!thread.joinable() && pending.empty()) {
        // When there are no pending requests, we're going to allow the queue to stop.
        queue->unref();
    }
}

}
