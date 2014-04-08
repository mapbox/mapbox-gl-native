#include "request.hpp"

#include <llmr/platform/platform.hpp>
#include <llmr/util/uv.hpp>
#include <llmr/util/loop.hpp>

#include <boost/lockfree/queue.hpp>

#include <curl/curl.h>

// This file contains code from http://curl.haxx.se/libcurl/c/multi-uv.html:

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2013, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* Example application code using the multi socket interface to download
   multiple files at once, but instead of using curl_multi_perform and
   curl_multi_wait, which uses select(), we use libuv.
   It supports epoll, kqueue, etc. on unixes and fast IO completion ports on
   Windows, which means, it should be very fast on all platforms..

   Written by Clemens Gruber, based on an outdated example from uvbook and
   some tests from libuv.

   Requires libuv and (of course) libcurl.

   See http://nikhilm.github.com/uvbook/ for more information on libuv.
*/

namespace llmr {
namespace platform {
namespace request {

struct curl_context {
    uv_poll_t poll_handle;
    curl_socket_t sockfd;
};

// Handles the request thread + messaging to the thread.
static uv_thread_t thread;
static uv::loop loop;
static uv::once init_thread;
static uv_async_t async_add;
static boost::lockfree::queue<llmr::platform::Request *> queue(8);

// Used as the CURL timer function to periodically check for socket updates.
static uv_timer_t timeout;

// CURL multi handle that we use to request multiple URLs at the same time,
// without having to block and spawn threads.
static CURLM *curl_multi = nullptr;

// CURL share handles are used for sharing session state (e.g.)
static uv::mutex curl_share_mutex;
static CURLSH *curl_share = nullptr;

// A queue that we use for storing resuable CURL easy handles to avoid creating
// and destroying them all the time.
static std::queue<CURL *> curl_handle_cache;

// Implementation starts here.

// Locks the CURL share handle
void curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *) { curl_share_mutex.lock(); }

// Unlocks the CURL share handle
void curl_share_unlock(CURL *, curl_lock_data, void *) { curl_share_mutex.unlock(); }

curl_context *create_curl_context(curl_socket_t sockfd) {
    curl_context *context = new curl_context;
    context->sockfd = sockfd;

    uv_poll_init_socket(*loop, &context->poll_handle, sockfd);
    context->poll_handle.data = context;

    return context;
}

void curl_close_cb(uv_handle_t *handle) {
    curl_context *context = (curl_context *)handle->data;
    free(context);
}

void destroy_curl_context(curl_context *context) {
    uv_close((uv_handle_t *)&context->poll_handle, curl_close_cb);
}

void curl_perform(uv_poll_t *req, int status, int events) {
    int running_handles;
    int flags = 0;
    curl_context *context;
    CURLMsg *message;
    int pending;

    uv_timer_stop(&timeout);

    if (events & UV_READABLE)
        flags |= CURL_CSELECT_IN;
    if (events & UV_WRITABLE)
        flags |= CURL_CSELECT_OUT;

    context = (curl_context *)req;

    curl_multi_socket_action(curl_multi, context->sockfd, flags, &running_handles);

    while ((message = curl_multi_info_read(curl_multi, &pending))) {
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

            CURLMcode error = curl_multi_remove_handle(curl_multi, handle);
            if (error != CURLM_OK) {
                fprintf(stderr, "CURL multi error: %s\n", curl_multi_strerror(error));
                break;
            }

            curl_easy_reset(handle);
            curl_handle_cache.push(handle);

            break;
        }

        default:
            // This should never happen, because there are no other message types.
            fprintf(stderr, "CURLMSG returned unknown message type\n");
            abort();
        }
    }
}

int handle_socket(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp) {
    curl_context *context;
    if (action == CURL_POLL_IN || action == CURL_POLL_OUT) {
        if (socketp) {
            context = (curl_context *)socketp;
        } else {
            context = create_curl_context(s);
        }
        curl_multi_assign(curl_multi, s, (void *)context);
    }

    switch (action) {
    case CURL_POLL_IN:
        uv_poll_start(&context->poll_handle, UV_READABLE, curl_perform);
        break;
    case CURL_POLL_OUT:
        uv_poll_start(&context->poll_handle, UV_WRITABLE, curl_perform);
        break;
    case CURL_POLL_REMOVE:
        if (socketp) {
            uv_poll_stop(&((curl_context *)socketp)->poll_handle);
            destroy_curl_context((curl_context *)socketp);
            curl_multi_assign(curl_multi, s, NULL);
        }
        break;
    default:
        abort();
    }

    return 0;
}

void on_timeout(uv_timer_t *req, int status) {
    int running_handles;
    CURLMcode error =
        curl_multi_socket_action(curl_multi, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    if (error != CURLM_OK) {
        fprintf(stderr, "CURL multi error: %s\n", curl_multi_strerror(error));
    }
}

void start_timeout(CURLM *multi, long timeout_ms, void *userp) {
    if (timeout_ms <= 0) {
        on_timeout(&timeout, 0);
    } else {
        uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
    }
}

// This function is the first function called in the request thread. It sets up the CURL share/multi
// handles and runs the thread loop.
void thread_init(void *ptr) {
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

    CURLMcode multi_error;
    curl_multi = curl_multi_init();

    multi_error = curl_multi_setopt(curl_multi, CURLMOPT_SOCKETFUNCTION, handle_socket);
    if (multi_error != CURLM_OK) {
        fprintf(stderr, "CURL multi error: %s\n", curl_multi_strerror(multi_error));
    }
    multi_error = curl_multi_setopt(curl_multi, CURLMOPT_TIMERFUNCTION, start_timeout);
    if (multi_error != CURLM_OK) {
        fprintf(stderr, "CURL multi error: %s\n", curl_multi_strerror(multi_error));
    }

    // Main event loop. This will not return until the request loop is terminated.
    loop.run();

    curl_multi_cleanup(curl_multi);
    curl_multi = nullptr;
    curl_share_cleanup(curl_share);
    curl_share = nullptr;

    // Clean up all the CURL easy handles that we kept around for potential future reuse.
    while (!curl_handle_cache.empty()) {
        curl_easy_cleanup(curl_handle_cache.front());
        curl_handle_cache.pop();
    }
}

// This function is called when we have new data for a request. We just append it to the string
// containing the previous data.
size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// This callback is called in the request event loop (on the request thread).
// It initializes newly queued up download requests and adds them to the CURL
// multi handle.
void async_add_cb(uv_async_t *async, int status) {
    Request *req = nullptr;
    while (queue.pop(req)) {
        req->res = new Response();

        // Obtain a curl handle (and try to reuse existing handles before creating new ones).
        CURL *handle = nullptr;
        if (!curl_handle_cache.empty()) {
            handle = curl_handle_cache.front();
            curl_handle_cache.pop();
        } else {
            handle = curl_easy_init();
        }

        curl_easy_setopt(handle, CURLOPT_PRIVATE, req);
        curl_easy_setopt(handle, CURLOPT_URL, req->url.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &req->res->body);
        curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "deflate");
        curl_easy_setopt(handle, CURLOPT_SHARE, curl_share);
        curl_multi_add_handle(curl_multi, handle);
    }
}
} // end namespace request
} // end namespace platform
} // end namespace llmr

using namespace llmr::platform;

Request::Request(std::string url, std::function<void(Response *)> fn) : url(url), fn(fn) {
    using namespace request;
    init_thread([]() {
        uv_async_init(*loop, &async_add, async_add_cb);
        uv_thread_create(&thread, thread_init, nullptr);
    });
    queue.push(this);
    uv_async_send(&async_add);
}
