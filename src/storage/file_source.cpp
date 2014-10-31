#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/util/uv-messenger.h>

#include <uv.h>

namespace mbgl {

FileSource::FileSource(uv_loop_t *loop_, const std::string &path)
    : thread_id(uv_thread_self()),
      store(!path.empty() ? util::ptr<SQLiteStore>(new SQLiteStore(loop_, path)) : nullptr),
      loop(loop_),
      queue(new uv_messenger_t) {

    uv_messenger_init(loop, queue, [](void *ptr) {
        std::unique_ptr<std::function<void()>> fn { reinterpret_cast<std::function<void()> *>(ptr) };
        (*fn)();
    });
    uv_unref((uv_handle_t *)&queue->async);
}

FileSource::~FileSource() {
    assert(thread_id == uv_thread_self());
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

void FileSource::setBase(const std::string &value) {
    assert(thread_id == uv_thread_self());
    base = value;
}

const std::string &FileSource::getBase() const {
    assert(thread_id == uv_thread_self());
    return base;
}

std::unique_ptr<Request> FileSource::request(ResourceType type, const std::string &url) {
    assert(thread_id == uv_thread_self());

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

    return std::unique_ptr<Request>(new Request(req));
}

void FileSource::prepare(std::function<void()> fn) {
    if (thread_id == uv_thread_self()) {
        fn();
    } else {
        uv_messenger_send(queue, new std::function<void()>(std::move(fn)));
    }
}

void FileSource::retryAllPending() {
    assert(thread_id == uv_thread_self());

    util::ptr<BaseRequest> req;
    for (const std::pair<std::string, std::weak_ptr<BaseRequest>> &pair : pending) {
        if ((req = pair.second.lock())) {
            req->retryImmediately();
        }
    }

}

}