#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/util/uv-messenger.h>

#include <uv.h>

namespace mbgl {

FileSource::FileSource(uv_loop_t *loop_)
    : thread_id(uv_thread_self()),
      store(util::ptr<SQLiteStore>(new SQLiteStore(loop_, "cache.db"))),
      loop(loop_),
      queue(new uv_messenger_t) {

    uv_messenger_init(loop, queue, [](void *ptr) {
        std::unique_ptr<std::function<void()>> fn { reinterpret_cast<std::function<void()> *>(ptr) };
        (*fn)();
    });
}

FileSource::~FileSource() {
    assert(thread_id == uv_thread_self());
    uv_messenger_stop(queue);
    // NOTE: We don't need to delete the messenger since it will be deleted by the
    // uv_messenger_stop() function.
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

    util::ptr<BaseRequest> request;

    // First, try to find an existing Request object.
    auto it = pending.find(absoluteURL);
    if (it != pending.end()) {
        request = it->second.lock();
    }

    if (!request) {
        if (absoluteURL.substr(0, 7) == "file://") {
            request = std::make_shared<FileRequest>(absoluteURL.substr(7), loop);
        } else {
            request = std::make_shared<HTTPRequest>(type, absoluteURL, loop, store);
        }

        pending.emplace(absoluteURL, request);
    }

    return std::unique_ptr<Request>(new Request(request));
}

void FileSource::prepare(std::function<void()> fn) {
    if (thread_id == uv_thread_self()) {
        fn();
    } else {
        uv_messenger_send(queue, new std::function<void()>(std::move(fn)));
    }
}

}