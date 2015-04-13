#include <mbgl/storage/default_file_source_impl.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/http_request.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/platform.hpp>

#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/platform/log.hpp>

#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <algorithm>
#include <cassert>


namespace algo = boost::algorithm;

namespace mbgl {

DefaultFileSource::Impl::Impl(FileCache* cache_, const std::string& root)
    : assetRoot(root.empty() ? platform::assetRoot() : root), cache(cache_) {
#ifdef __APPLE__
    pthread_setname_np("FileSource");
#endif
}

DefaultFileSource::DefaultFileSource(FileCache* cache, const std::string& root)
    : thread(util::make_unique<util::Thread<Impl>>(cache, root)) {
}

DefaultFileSource::~DefaultFileSource() {
    MBGL_VERIFY_THREAD(tid);
}

SharedRequestBase *DefaultFileSource::Impl::find(const Resource &resource) {
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

Request* DefaultFileSource::request(const Resource& resource,
                                    uv_loop_t* l,
                                    const Environment& env,
                                    Callback callback) {
    auto req = new Request(resource, l, env, std::move(callback));

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue.
    (*thread)->invoke([=] { (*thread)->processAdd(req); });

    return req;
}

void DefaultFileSource::request(const Resource& resource, const Environment& env, Callback callback) {
    request(resource, nullptr, env, std::move(callback));
}

void DefaultFileSource::cancel(Request *req) {
    req->cancel();

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue.
    (*thread)->invoke([=] { (*thread)->processCancel(req); });
}

void DefaultFileSource::abort(const Environment &env) {
    (*thread)->invoke([=, &env] { (*thread)->processAbort(env); });
}

void DefaultFileSource::Impl::processAdd(Request* req) {
    const Resource &resource = req->resource;

    // We're adding a new Request.
    SharedRequestBase *sharedRequest = find(resource);
    if (!sharedRequest) {
        // There is no request for this URL yet. Create a new one and start it.
        if (algo::starts_with(resource.url, "asset://")) {
            sharedRequest = new AssetRequest(this, resource);
        } else {
            sharedRequest = new HTTPRequest(this, resource);
        }

        const bool inserted = pending.emplace(resource, sharedRequest).second;
        assert(inserted);
        (void (inserted)); // silence unused variable warning on Release builds.

        // But first, we're going to start querying the database if it exists.
        if (!cache) {
            sharedRequest->start(loop().get());
        } else {
            // Otherwise, first check the cache for existing data so that we can potentially
            // revalidate the information without having to redownload everything.
            cache->get(resource, [this, resource](std::unique_ptr<Response> response) {
                std::shared_ptr<const Response> sharedResponse = std::move(response);
                invoke([this, resource, sharedResponse] { processResult(resource, sharedResponse); });
            });
        }
    }
    sharedRequest->subscribe(req);
}

void DefaultFileSource::Impl::processCancel(Request* req) {
    SharedRequestBase *sharedRequest = find(req->resource);
    if (sharedRequest) {
        // If the number of dependent requests of the SharedRequestBase drops to zero, the
        // unsubscribe callback triggers the removal of the SharedRequestBase pointer from the list
        // of pending requests and initiates cancelation.
        sharedRequest->unsubscribe(req);
    } else {
        // There is no request for this URL anymore. Likely, the request already completed
        // before we got around to process the cancelation request.
    }

    // Send a message back to the requesting thread and notify it that this request has been
    // canceled and is now safe to be deleted.
    req->destruct();
}

void DefaultFileSource::Impl::processResult(const Resource& resource, std::shared_ptr<const Response> response) {
    SharedRequestBase *sharedRequest = find(resource);
    if (sharedRequest) {
        if (response) {
            // This entry was stored in the cache. Now determine if we need to revalidate.
            const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                    SystemClock::now().time_since_epoch()).count();
            if (response->expires > now) {
                // The response is fresh. We're good to notify the caller.
                sharedRequest->notify(response, FileCache::Hint::No);
                sharedRequest->cancel();
                return;
            } else {
                // The cached response is stale. Now run the real request.
                sharedRequest->start(loop().get(), response);
            }
        } else {
            // There is no response. Now run the real request.
            sharedRequest->start(loop().get());
        }
    } else {
        // There is no request for this URL anymore. Likely, the request was canceled
        // before we got around to process the cache result.
    }
}

// Aborts all requests that are part of the current environment.
void DefaultFileSource::Impl::processAbort(const Environment& env) {
    // Construct a cancellation response.
    auto res = util::make_unique<Response>();
    res->status = Response::Error;
    res->message = "Environment is terminating";
    std::shared_ptr<const Response> response = std::move(res);

    // Iterate through all pending requests and remove them in case they're abandoned.
    util::erase_if(pending, [&](const std::pair<Resource, SharedRequestBase *> &it) -> bool {
        // Obtain all pending requests that are in the current environment.
        const auto aborted = it.second->removeAllInEnvironment(env);

        // Notify all observers.
        for (auto req : aborted) {
            req->notify(response);
        }

        // Finally, remove all requests that are now abandoned.
        if (it.second->abandoned()) {
            it.second->cancel();
            return true;
        } else {
            return false;
        }
    });
}

void DefaultFileSource::Impl::notify(SharedRequestBase *sharedRequest,
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
        for (auto req : observers) {
            req->notify(response);
        }
    }
}

}
