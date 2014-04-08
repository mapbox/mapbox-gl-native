#include "request.hpp"

// #include <atomic>
 #include <cassert>

#include <uv.h>
#include <cstdlib>
// #include <llmr/util/threadpool.hpp>
#include <llmr/platform/platform.hpp>

namespace llmr {
namespace platform {

typedef struct curl_context_s {
    uv_poll_t poll_handle;
    curl_socket_t sockfd;
} curl_context_t;
}
}

using namespace llmr::platform;

size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

uv_once_t Request::init_thread = UV_ONCE_INIT;
uv::loop Request::loop;
uv_thread_t Request::thread;
uv_timer_t Request::timeout;
uv_async_t Request::async_add;
CURLM *Request::curl_handle = nullptr;

uv::mutex Request::curl_share_mutex;
CURLSH *Request::curl_share = nullptr;

uv::mutex Request::request_mutex;
std::queue<Request *> Request::requests;

std::queue<CURL *> Request::curl_handles;

Request::Request(std::string url, std::function<void(Response *)> fn) : url(url), fn(fn) {
    uv_once(&init_thread, init_thread_cb);

    {
        uv::lock lock(request_mutex);
        requests.push(this);
    }

    uv_async_send(&async_add);
}

void Request::init_thread_cb() {
    uv_async_init(*loop, &async_add, async_add_cb);
    uv_thread_create(&thread, init_thread_run, nullptr);
}

curl_context_t *Request::create_curl_context(curl_socket_t sockfd) {
    curl_context_t *context;

    context = (curl_context_t *)malloc(sizeof *context);

    context->sockfd = sockfd;

    uv_poll_init_socket(*loop, &context->poll_handle, sockfd);
    context->poll_handle.data = context;

    return context;
}

void curl_close_cb(uv_handle_t *handle) {
    curl_context_t *context = (curl_context_t *)handle->data;
    free(context);
}

void Request::destroy_curl_context(curl_context_t *context) {
    uv_close((uv_handle_t *)&context->poll_handle, curl_close_cb);
}

int Request::handle_socket(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp) {
    curl_context_t *curl_context;
    if (action == CURL_POLL_IN || action == CURL_POLL_OUT) {
        if (socketp) {
            curl_context = (curl_context_t *)socketp;
        } else {
            curl_context = create_curl_context(s);
        }
        curl_multi_assign(curl_handle, s, (void *)curl_context);
    }

    switch (action) {
    case CURL_POLL_IN:
        uv_poll_start(&curl_context->poll_handle, UV_READABLE, curl_perform);
        break;
    case CURL_POLL_OUT:
        uv_poll_start(&curl_context->poll_handle, UV_WRITABLE, curl_perform);
        break;
    case CURL_POLL_REMOVE:
        if (socketp) {
            uv_poll_stop(&((curl_context_t *)socketp)->poll_handle);
            destroy_curl_context((curl_context_t *)socketp);
            curl_multi_assign(curl_handle, s, NULL);
        }
        break;
    default:
        abort();
    }

    return 0;
}

void Request::curl_perform(uv_poll_t *req, int status, int events) {
    int running_handles;
    int flags = 0;
    curl_context_t *context;
    CURLMsg *message;
    int pending;

    uv_timer_stop(&timeout);

    if (events & UV_READABLE)
        flags |= CURL_CSELECT_IN;
    if (events & UV_WRITABLE)
        flags |= CURL_CSELECT_OUT;

    context = (curl_context_t *)req;

    curl_multi_socket_action(curl_handle, context->sockfd, flags, &running_handles);

    while ((message = curl_multi_info_read(curl_handle, &pending))) {
        switch (message->msg) {
        case CURLMSG_DONE: {
            long code;
            Request *req = nullptr;
            curl_easy_getinfo(message->easy_handle, CURLINFO_RESPONSE_CODE, &code);
            curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, (char *)&req);

            req->res->code = code;
            req->fn(req->res);
            delete req->res;
            delete req;


            CURL *handle = message->easy_handle;

            CURLMcode error = curl_multi_remove_handle(curl_handle, handle);
            if (error != CURLM_OK) {
                fprintf(stderr, "CURL multi error: %s\n", curl_multi_strerror(error));
                break;
            }

            curl_easy_reset(handle);
            curl_handles.push(handle);

            break;
        }

        default:
            fprintf(stderr, "CURLMSG default\n");
            abort();
        }
    }
}

void Request::on_timeout(uv_timer_t *req, int status) {
    int running_handles;
    CURLMcode error = curl_multi_socket_action(curl_handle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    if (error != CURLM_OK) {
        fprintf(stderr, "CURL multi error: %s\n", curl_multi_strerror(error));
    }
}

void Request::start_timeout(CURLM *multi, long timeout_ms, void *userp) {
    if (timeout_ms <= 0) {
        // 0 means directly call socket_action, but we'll do it in a bit
        timeout_ms = 1;
    }
    uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
}

void Request::init_thread_run(void *ptr) {
    uv_timer_init(*loop, &timeout);

    CURLSHcode share_error;
    curl_share = curl_share_init();

    share_error = curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, curl_share_lock);
    if (share_error != CURLSHE_OK) {
        fprintf(stderr, "CURL share error: %s\n", curl_share_strerror(share_error));
    }

    share_error = curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, curl_share_unlock);
    if (share_error != CURLSHE_OK) {
        fprintf(stderr, "CURL share error: %s\n", curl_share_strerror(share_error));
    }

    curl_handle = curl_multi_init();
    curl_multi_setopt(curl_handle, CURLMOPT_SOCKETFUNCTION, handle_socket);
    curl_multi_setopt(curl_handle, CURLMOPT_TIMERFUNCTION, start_timeout);

    loop.run();

    curl_multi_cleanup(curl_handle);
    curl_share_cleanup(curl_share);

    while (!curl_handles.empty()) {
        curl_easy_cleanup(curl_handles.front());
        curl_handles.pop();
    }
}

void Request::async_add_cb(uv_async_t *async, int status) {
    request_mutex.lock();

    while (requests.size()) {
        Request *req = requests.front();
        requests.pop();
        request_mutex.unlock();

        req->res = new Response();

        // Obtain a curl handle (and try to reuse existing handles before creating new ones)
        CURL *handle = nullptr;
        if (!curl_handles.empty()) {
            handle = curl_handles.front();
            curl_handles.pop();
        } else {
            handle = curl_easy_init();
        }

        curl_easy_setopt(handle, CURLOPT_PRIVATE, req);
        curl_easy_setopt(handle, CURLOPT_URL, req->url.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &req->res->body);
        curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "deflate");
        curl_easy_setopt(handle, CURLOPT_SHARE, curl_share);
        curl_multi_add_handle(curl_handle, handle);

        request_mutex.lock();
    }

    request_mutex.unlock();
}

void Request::curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *) {
    curl_share_mutex.lock();
}

void Request::curl_share_unlock(CURL *, curl_lock_data, void *) {
    curl_share_mutex.unlock();
}
