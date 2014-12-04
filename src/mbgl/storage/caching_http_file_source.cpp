#include <mbgl/storage/caching_http_file_source.hpp>
#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/std.hpp>

#include <uv.h>

namespace mbgl {

CachingHTTPFileSource::CachingHTTPFileSource(const std::string &path_)
    : path(path_) {}

CachingHTTPFileSource::~CachingHTTPFileSource() {
}

void CachingHTTPFileSource::setLoop(uv_loop_t* loop_) {
    thread_id = std::this_thread::get_id();
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
    assert(thread_id == std::this_thread::get_id());
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

void CachingHTTPFileSource::setBase(const std::string &value) {
    assert(thread_id == std::this_thread::get_id());
    base = value;
}

const std::string &CachingHTTPFileSource::getBase() const {
    assert(thread_id == std::this_thread::get_id());
    return base;
}

std::unique_ptr<Request> CachingHTTPFileSource::request(ResourceType type, const std::string &url) {
    assert(thread_id == std::this_thread::get_id());

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
        } else {
            req = std::make_shared<HTTPRequest>(type, absoluteURL, loop, store);
        }

        pending.emplace(absoluteURL, req);
    }

    return util::make_unique<Request>(req);
}

void CachingHTTPFileSource::prepare(std::function<void()> fn) {
    if (thread_id == std::this_thread::get_id()) {
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
