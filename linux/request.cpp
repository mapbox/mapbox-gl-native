#include "request.hpp"

#include <atomic>
#include <cassert>

#include <llmr/util/threadpool.hpp>
#include <llmr/platform/platform.hpp>

using namespace llmr::platform;


llmr::util::Threadpool *Request::pool = nullptr;
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
    bool cancel = req->cancelled;
    if (cancel) {
        fprintf(stderr, "cancel download %s\n", req->url.c_str());
    }
    return cancel;
}

void Request::initialize() {
    pool = new llmr::util::Threadpool(Request::request);

    // curl init
    curl_global_init(CURL_GLOBAL_ALL);
    curl_share = curl_share_init();
    curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, curl_share_lock);
    curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, curl_share_unlock);
}

void Request::finish() {
    delete pool;
    curl_share_cleanup(curl_share);
}

Request::Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg)
    : done(false),
      cancelled(false),
      url(url),
      background_function(bg),
      foreground_callback(fg) {
    assert(pool);
    pool->add(this);
}

void Request::request(void *ptr) {
    assert(curl_share);

    Request *req = static_cast<Request *>(ptr);
    Response res;

    // TODO: use curl multi to be able to cancel, or to

    CURL *curl = curl_easy_init();
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
    curl_easy_cleanup(curl);

    if (code != CURLE_ABORTED_BY_CALLBACK) {
        req->background_function(res);
    }

    req->done = true;
}

void Request::cancel() {
    cancelled = true;
}
