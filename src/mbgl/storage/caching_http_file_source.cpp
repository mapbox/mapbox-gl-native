#include <mbgl/storage/caching_http_file_source.hpp>
#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/std.hpp>

#include <uv.h>

namespace mbgl {

CachingHTTPFileSource::CachingHTTPFileSource(const std::string &path_)
    : path(path_) {}

CachingHTTPFileSource::~CachingHTTPFileSource() {
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

void CachingHTTPFileSource::clearLoop() {
    assert(std::this_thread::get_id() == threadId);
    assert(loop);

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

    store.reset();
}

void CachingHTTPFileSource::setBase(std::string value) {
    // TODO: Make threadsafe.
    base.swap(value);
}

void CachingHTTPFileSource::setAccessToken(std::string value) {
    // TODO: Make threadsafe.
    accessToken.swap(value);
}

std::unique_ptr<Request> CachingHTTPFileSource::request(ResourceType type, const std::string& url_) {
    assert(tstd::this_thread::get_id() == threadId);

    std::string url = url_;

    // Make URL absolute.
    const size_t separator = url.find("://");
    if (separator == std::string::npos) {
        url = base + url;
    }

    // Normalize mapbox:// URLs.
    switch (type) {
    case ResourceType::Glyphs:
        url = util::mapbox::normalizeGlyphsURL(url, accessToken);
    default:
        url = util::mapbox::normalizeSourceURL(url, accessToken);
    }

    util::ptr<BaseRequest> req;

    // First, try to find an existing Request object.
    auto it = pending.find(url);
    if (it != pending.end()) {
        req = it->second.lock();
    }

    if (!req) {
        if (url.substr(0, 7) == "file://") {
            req = std::make_shared<FileRequest>(url.substr(7), loop);
        } else if (url.substr(0, 8) == "asset://") {
            req = std::make_shared<AssetRequest>(url.substr(8), loop);
        } else {
            req = std::make_shared<HTTPRequest>(type, url, loop, store);
        }

        pending.emplace(url, req);
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

void CachingHTTPFileSource::setReachability(bool reachable) {
    if (reachable && loop) {
        prepare([this]() {
            util::ptr<BaseRequest> req;
            for (const std::pair<std::string, std::weak_ptr<BaseRequest>> &pair : pending) {
                if ((req = pair.second.lock())) {
                    req->retryImmediately();
                }
            }
        });
    }
}

}
