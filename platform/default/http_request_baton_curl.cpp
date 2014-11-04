#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/std.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/time.hpp>

#include <uv.h>
#include <curl/curl.h>

#include <queue>
#include <cassert>
#include <cstring>

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

// Handles the request thread + messaging to the thread.
static uv_once_t once;
static uv_loop_t *loop = nullptr;
static uv_messenger_t start_messenger;
static uv_messenger_t stop_messenger;
static uv_thread_t thread;
static unsigned long thread_id;

// Used as the CURL timer function to periodically check for socket updates.
static uv_timer_t timeout;

// CURL multi handle that we use to request multiple URLs at the same time, without having to block
// and spawn threads.
static CURLM *multi = nullptr;

// CURL share handles are used for sharing session state (e.g.)
static uv_mutex_t share_mutex;
static CURLSH *share = nullptr;

// A queue that we use for storing resuable CURL easy handles to avoid creating and destroying them
// all the time.
static std::queue<CURL *> handles;

namespace mbgl {

struct CURLContext {
    util::ptr<HTTPRequestBaton> baton;
    uv_poll_t *poll_handle = nullptr;
    curl_socket_t sockfd = 0;
    curl_slist *headers = nullptr;

    CURLContext(const util::ptr<HTTPRequestBaton> &baton_) : baton(baton_) {
    }

    ~CURLContext() {
        // We are destructing the poll handle in a CURL callback already.
        assert(!poll_handle);

        // Once the CURLContext gets destroyed, CURL doesn't need any headers anymore.
        if (headers) {
            curl_slist_free_all(headers);
            headers = nullptr;
        }
    }
};

// Locks the CURL share handle
void curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *) {
    uv_mutex_lock(&share_mutex);
}

// Unlocks the CURL share handle
void curl_share_unlock(CURL *, curl_lock_data, void *) {
    uv_mutex_unlock(&share_mutex);
}

// This function must run in the CURL thread.
// It is either called when the request is completed, or when we try to cancel the request.
void finish_request(const util::ptr<HTTPRequestBaton> &baton) {
    assert(uv_thread_self() == thread_id);
    if (baton->ptr) {
        CURL *handle = (CURL *)baton->ptr;
        CURLMcode error = curl_multi_remove_handle(multi, handle);
        if (error != CURLM_OK) {
            baton->response = std::make_unique<Response>();
            baton->response->code = -1;
            baton->response->message = curl_multi_strerror(error);
        }

        // Destroy the shared pointer. We still have one pointing to it
        CURLContext *context = nullptr;
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, (char *)&context);
        curl_easy_setopt(handle, CURLOPT_PRIVATE, nullptr);
        delete context;

        // TODO: delete the headers object again.

        curl_easy_reset(handle);
        handles.push(handle);
        baton->ptr = nullptr;
    }
}

void curl_perform(uv_poll_t *req, int, int events) {
    int running_handles;
    int flags = 0;
    CURLContext *context = (CURLContext *)req->data;
    CURLMsg *message;
    int pending;

    uv_timer_stop(&timeout);

    if (events & UV_READABLE) {
        flags |= CURL_CSELECT_IN;
    }
    if (events & UV_WRITABLE) {
        flags |= CURL_CSELECT_OUT;
    }

    curl_multi_socket_action(multi, context->sockfd, flags, &running_handles);

    while ((message = curl_multi_info_read(multi, &pending))) {
        switch (message->msg) {
        case CURLMSG_DONE: {
            CURLContext *ctx = nullptr;
            curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, (char *)&ctx);

            // Make a copy so that the Baton stays around even after we are calling finish_request
            util::ptr<HTTPRequestBaton> baton = ctx->baton;

            // Add human-readable error code
            if (message->data.result != CURLE_OK) {
                baton->response->message = curl_easy_strerror(message->data.result);
                baton->response->code = -1;

                switch (message->data.result) {
                    case CURLE_COULDNT_RESOLVE_PROXY:
                    case CURLE_COULDNT_RESOLVE_HOST:
                    case CURLE_COULDNT_CONNECT:
                        baton->type = HTTPResponseType::ConnectionError;
                        break;

                    case CURLE_OPERATION_TIMEDOUT:
                        baton->type = HTTPResponseType::TemporaryError;
                        break;

                    default:
                        baton->type = HTTPResponseType::PermanentError;
                }
            } else {
                long code = 0;
                curl_easy_getinfo(message->easy_handle, CURLINFO_RESPONSE_CODE, &code);

                if (code != 304) {
                    baton->response->code = code;
                }

                if (code == 304) {
                    baton->type = HTTPResponseType::NotModified;
                } else if (code == 200) {
                    baton->type = HTTPResponseType::Successful;
                } else if (code >= 500 && code < 600) {
                    baton->type = HTTPResponseType::TemporaryError;
                } else if (code >= 400 && code < 500) {
                    baton->type = HTTPResponseType::PermanentError;
                } else {
                    assert(!"code must be either 200 or 304");
                }
            }

            // We're currently in the CURL request thread.
            finish_request(baton);

            if (baton->async) {
                uv_async_send(baton->async);
                baton->async = nullptr;
            }

            break;
        }

        default:
            // This should never happen, because there are no other message types.
            throw std::runtime_error("CURLMSG returned unknown message type");
        }
    }
}

int handle_socket(CURL *handle, curl_socket_t sockfd, int action, void *, void *socketp) {
    CURLContext *context = nullptr;
    curl_easy_getinfo(handle, CURLINFO_PRIVATE, (char *)&context);

    if (!socketp && action != CURL_POLL_REMOVE) {
        // We haven't initialized the socket yet, so we need to do this now.
        assert(context->sockfd == 0);
        context->sockfd = sockfd;
        assert(!context->poll_handle);
        context->poll_handle = new uv_poll_t;
        uv_poll_init_socket(loop, context->poll_handle, sockfd);
        context->poll_handle->data = context;
        curl_multi_assign(multi, sockfd, context);
    }

    if (context) {
        if (action == CURL_POLL_IN || action == CURL_POLL_INOUT) {
            uv_poll_start(context->poll_handle, UV_READABLE, curl_perform);
        }
        if (action == CURL_POLL_OUT || action == CURL_POLL_INOUT) {
            uv_poll_start(context->poll_handle, UV_WRITABLE, curl_perform);
        }
        if (action == CURL_POLL_REMOVE && socketp) {
            uv_poll_stop(context->poll_handle);
            uv_close((uv_handle_t *)context->poll_handle, [](uv_handle_t *poll_handle) {
                delete (uv_poll_t *)poll_handle;
            });
            context->poll_handle = nullptr;
            curl_multi_assign(multi, sockfd, NULL);
        }
    }

    return 0;
}

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
void on_timeout(uv_timer_t *, int = 0) {
#else
void on_timeout(uv_timer_t *) {
#endif
    int running_handles;
    CURLMcode error = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    if (error != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(error));
    }
}

void start_timeout(CURLM *, long timeout_ms, void *) {
    if (timeout_ms <= 0) {
        on_timeout(&timeout, 0);
    } else {
        uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
    }
}

void thread_init(void *) {
#ifdef __APPLE__
    pthread_setname_np("CURL");
#endif
    thread_id = uv_thread_self();

    uv_timer_init(loop, &timeout);

    CURLSHcode share_error;
    share = curl_share_init();

    share_error = curl_share_setopt(share, CURLSHOPT_LOCKFUNC, curl_share_lock);
    if (share_error != CURLSHE_OK) {
        throw std::runtime_error(std::string("CURL share error: ") + curl_share_strerror(share_error));
    }

    share_error = curl_share_setopt(share, CURLSHOPT_UNLOCKFUNC, curl_share_unlock);
    if (share_error != CURLSHE_OK) {
        throw std::runtime_error(std::string("CURL share error: ") + curl_share_strerror(share_error));
    }


    CURLMcode multi_error;
    multi = curl_multi_init();

    multi_error = curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, handle_socket);
    if (multi_error != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(multi_error));
    }
    multi_error = curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, start_timeout);
    if (multi_error != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(multi_error));

    }

    // Main event loop. This will not return until the request loop is terminated.
    uv_run(loop, UV_RUN_DEFAULT);

    curl_multi_cleanup(multi);
    multi = nullptr;

    curl_share_cleanup(share);
    share = nullptr;

    thread_id = -1;
}

// This function is called when we have new data for a request. We just append it to the string
// containing the previous data.
size_t curl_write_cb(void *const contents, const size_t size, const size_t nmemb, void *const userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// Compares the beginning of the (non-zero-terminated!) data buffer with the (zero-terminated!)
// header string. If the data buffer contains the header string at the beginning, it returns
// the length of the header string == begin of the value, otherwise it returns npos.
// The comparison of the header is ASCII-case-insensitive.
size_t header_matches(const char *const header, const char *const buffer, const size_t length) {
    const size_t header_length = strlen(header);
    if (length < header_length) return std::string::npos;
    size_t i = 0;
    while (i < length && i < header_length && std::tolower(buffer[i]) == header[i]) {
        i++;
    }
    return i == header_length ? i : std::string::npos;
}

size_t curl_header_cb(char * const buffer, const size_t size, const size_t nmemb, void *const userp) {
    const size_t length = size * nmemb;

    Response *response = static_cast<Response *>(userp);

    size_t begin = std::string::npos;
    if ((begin = header_matches("last-modified: ", buffer, length)) != std::string::npos) {
        // Always overwrite the modification date; We might already have a value here from the
        // Date header, but this one is more accurate.
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        response->modified = curl_getdate(value.c_str(), nullptr);
    } else if ((begin = header_matches("etag: ", buffer, length)) != std::string::npos) {
        response->etag = { buffer + begin, length - begin - 2 }; // remove \r\n
    } else if ((begin = header_matches("cache-control: ", buffer, length)) != std::string::npos) {
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        response->expires = Response::parseCacheControl(value.c_str());
    }

    return length;
}

// This function must run in the CURL thread.
void start_request(void *const ptr) {
    assert(uv_thread_self() == thread_id);
    std::unique_ptr<util::ptr<HTTPRequestBaton>> baton_guard { (util::ptr<HTTPRequestBaton> *)ptr };
    util::ptr<HTTPRequestBaton> &baton = *baton_guard.get();
    assert(baton);

    CURL *handle = nullptr;
    if (!handles.empty()) {
        handle = handles.front();
        handles.pop();
    } else {
        handle = curl_easy_init();
    }

    baton->ptr = handle;

    // Wrap this in a unique_ptr for now so that it destructs until we assign it the the CURL handle.
    std::unique_ptr<CURLContext> context = std::make_unique<CURLContext>(baton);

    if (baton->response) {
        if (!baton->response->etag.empty()) {
            const std::string header = std::string("If-None-Match: ") + baton->response->etag;
            context->headers = curl_slist_append(context->headers, header.c_str());
        } else if (baton->response->modified) {
            const std::string time = std::string("If-Modified-Since: ") +
                util::rfc1123(baton->response->modified);
            context->headers = curl_slist_append(context->headers, time.c_str());
        }
    }

    if (context->headers) {
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, context->headers);
    }

    if (!baton->response) {
        baton->response = std::make_unique<Response>();
    }

    // Carry on the shared pointer in the private information of the CURL handle.
    curl_easy_setopt(handle, CURLOPT_PRIVATE, context.release());
    curl_easy_setopt(handle, CURLOPT_CAINFO, "ca-bundle.crt");
    curl_easy_setopt(handle, CURLOPT_URL, baton->path.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &baton->response->data);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, curl_header_cb);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, baton->response.get());
    curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
    curl_easy_setopt(handle, CURLOPT_SHARE, share);

    // Start requesting the information.
    curl_multi_add_handle(multi, handle);
}

// This function must run in the CURL thread.
void stop_request(void *const ptr) {
    assert(uv_thread_self() == thread_id);
    std::unique_ptr<util::ptr<HTTPRequestBaton>> baton_guard { (util::ptr<HTTPRequestBaton> *)ptr };
    util::ptr<HTTPRequestBaton> &baton = *baton_guard.get();
    assert(baton);

    if (baton->async) {
        baton->type = HTTPResponseType::Canceled;

        // We can still stop the request because it is still in progress.
        finish_request(baton);

        uv_async_send(baton->async);
        baton->async = nullptr;
    } else {
        // If the async handle is gone, it means that the actual request has been completed before
        // we got a chance to cancel it. In this case, this is a no-op. It is likely that
        // the pointer below is the last lifeline of the HTTPRequestBaton. This means we're going
        // to delete the HTTPRequestBaton in the current (CURL) thread.
    }
}

void create_thread() {
    uv_mutex_init(&share_mutex);
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    loop = uv_loop_new();
#else
    loop = new uv_loop_t;
    uv_loop_init(loop);
#endif
    uv_messenger_init(loop, &start_messenger, start_request);
    uv_messenger_init(loop, &stop_messenger, stop_request);
    uv_thread_create(&thread, thread_init, nullptr);
}

// This function must be run from the main thread (== where the HTTPRequestBaton was created)
void HTTPRequestBaton::start(const util::ptr<HTTPRequestBaton> &ptr) {
    assert(uv_thread_self() == ptr->thread_id);
    uv_once(&once, create_thread);
    uv_messenger_send(&start_messenger, new util::ptr<HTTPRequestBaton>(ptr));
}

// This function must be run from the main thread (== where the HTTPRequestBaton was created)
void HTTPRequestBaton::stop(const util::ptr<HTTPRequestBaton> &ptr) {
    assert(uv_thread_self() == ptr->thread_id);
    uv_once(&once, create_thread);
    uv_messenger_send(&stop_messenger, new util::ptr<HTTPRequestBaton>(ptr));
}

}
