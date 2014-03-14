#include "request.hpp"

#include <atomic>
#include <cassert>

#include <llmr/util/threadpool.hpp>
#include <llmr/platform/platform.hpp>

using namespace llmr::platform;

void Request::initialize() {
}

void Request::finish() {
}

Request::Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg)
    : url(url),
      background_function(bg),
      foreground_callback(fg) {
    llmr::util::threadpool->add(request, this);
}

pthread_key_t Request::key;
pthread_once_t Request::key_once = PTHREAD_ONCE_INIT;

void Request::create_key() {
    pthread_key_create(&key, delete_key);
}

void Request::delete_key(void *ptr) {
    if (ptr != nullptr) {
        // curl_easy_cleanup(ptr);
    }
}

void Request::request(void *ptr) {
    // assert(curl_share);

    Request *req = static_cast<Request *>(ptr);
    Response res;

    // pthread_once(&key_once, create_key);
    // TODO: use curl multi to be able to cancel, or to
}

void Request::cancel() {
    cancelled = true;
}
