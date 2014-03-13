#include "request.hpp"

#include <atomic>
#include <cassert>

#include <llmr/util/threadpool.hpp>
#include <llmr/platform/platform.hpp>

using namespace llmr::platform;


CURLSH *Request::curl_share = nullptr;
pthread_mutex_t Request::curl_share_mutex = PTHREAD_MUTEX_INITIALIZER;

void Request::curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *) {
    pthread_mutex_lock(&curl_share_mutex);
}

void Request::curl_share_unlock(CURL *, curl_lock_data, void *) {
    pthread_mutex_unlock(&curl_share_mutex);
}

size_t Request::curl_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int Request::curl_progress_callback(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow) {
    Request *req = static_cast<Request *>(ptr);
    return req->cancelled;
}

void Request::initialize() {
    // curl init
    curl_global_init(CURL_GLOBAL_ALL);

    curl_share = curl_share_init();
    curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, curl_share_lock);
    curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, curl_share_unlock);
}

void Request::finish() {
    pthread_key_delete(key);
    curl_share_cleanup(curl_share);
}

Request::Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg)
    : done(false),
      cancelled(false),
      url(url),
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
        curl_easy_cleanup(ptr);
    }
}

void Request::request(void *ptr) {
    assert(curl_share);

    Request *req = static_cast<Request *>(ptr);
    Response res;

    pthread_once(&key_once, create_key);
    // TODO: use curl multi to be able to cancel, or to

    CURL *curl = nullptr;
    if ((curl = pthread_getspecific(key)) == nullptr) {
        curl = curl_easy_init();

        // stopgap to avoid libcurl crashes:
        // see https://stackoverflow.com/q/9191668
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        pthread_setspecific(key, curl);
    }

    curl_easy_reset(curl);

    CURLcode code;

    curl_easy_setopt(curl, CURLOPT_URL, req->url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res.body);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, curl_progress_callback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, req);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    curl_easy_setopt(curl, CURLOPT_SHARE, curl_share);
    code = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.code);

    if (code != CURLE_ABORTED_BY_CALLBACK) {
        req->background_function(res);
    }

    req->done = true;
}

void Request::cancel() {
    cancelled = true;
}
