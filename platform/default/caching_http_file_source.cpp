#include <mbgl/platform/default/caching_http_file_source.hpp>
#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/std.hpp>

#include <uv.h>

namespace mbgl {

CachingHTTPFileSource::CachingHTTPFileSource(const std::string &path_)
    : path(path_) {}

CachingHTTPFileSource::~CachingHTTPFileSource() {
    if (hasLoop()) {
        // FIXME temp fix for #608 crash
        // assert(thread_id == std::this_thread::get_id());
        uv_messenger_stop(queue, [](uv_messenger_t *msgr) {
            delete msgr;
        });

        util::ptr<BaseRequest> req;

        // Send a cancel() message to all requests that we are still holding.
        for (const std::pair<std::string, std::weak_ptr<BaseRequest>> &pair : pending) {
            if ((req = pair.second.lock())) {
                req->cancel();
            }
        }
    }
}

void CachingHTTPFileSource::setLoop(uv_loop_t* loop_) {
    threadId = std::this_thread::get_id();
    store = !path.empty() ? util::ptr<SQLiteStore>(new SQLiteStore(loop_, path)) : nullptr;
    loop = loop_;
    queue = new uv_messenger_t;

    uv_messenger_init(loop, queue, [](void *ptr) {
        std::unique_ptr<std::function<void()>> fn { reinterpret_cast<std::function<void()> *>(ptr) };
        (*fn)();
    });
    uv_unref((uv_handle_t *)&queue->async);
}

bool CachingHTTPFileSource::hasLoop() {
    return loop;
}

void CachingHTTPFileSource::setBase(const std::string &value) {
    assert(std::this_thread::get_id() == threadId);
    base = value;
}

const std::string &CachingHTTPFileSource::getBase() const {
    assert(std::this_thread::get_id() == threadId);
    return base;
}

std::unique_ptr<Request> CachingHTTPFileSource::request(ResourceType type, const std::string &url) {
    assert(std::this_thread::get_id() == threadId);

    // Make URL absolute.
    const std::string absoluteURL = [&]() -> std::string {
        const size_t separator = url.find("://");
        if (separator == std::string::npos) {
            // Relative URL.
            return base + url;
        } else {
            return url;
        }
    }();

    util::ptr<BaseRequest> req;

    // First, try to find an existing Request object.
    auto it = pending.find(absoluteURL);
    if (it != pending.end()) {
        req = it->second.lock();
    }

    if (!req) {
        if (absoluteURL.substr(0, 7) == "file://") {
            req = std::make_shared<FileRequest>(absoluteURL.substr(7), loop);
        } else if (absoluteURL.substr(0, 8) == "asset://") {
            req = std::make_shared<AssetRequest>(absoluteURL.substr(8), loop);
        } else {
            req = std::make_shared<HTTPRequest>(type, absoluteURL, loop, store);
        }

        pending.emplace(absoluteURL, req);
    }

    return util::make_unique<Request>(req);
}

void CachingHTTPFileSource::prepare(std::function<void()> fn) {
    if (std::this_thread::get_id() == threadId) {
        fn();
    } else {
        uv_messenger_send(queue, new std::function<void()>(std::move(fn)));
    }
}

void CachingHTTPFileSource::retryAllPending() {
    assert(std::this_thread::get_id() == threadId);

    util::ptr<BaseRequest> req;
    for (const std::pair<std::string, std::weak_ptr<BaseRequest>> &pair : pending) {
        if ((req = pair.second.lock())) {
            req->retryImmediately();
        }
    }

}

}
