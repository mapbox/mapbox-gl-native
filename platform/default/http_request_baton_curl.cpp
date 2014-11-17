#include <mbgl/mbgl.hpp>
#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/time.hpp>
#include <mbgl/util/string.hpp>

#ifdef __ANDROID__
    #include <mbgl/android/jni.hpp>
    #include <zip.h>
    #include <boost/make_unique.hpp>
    #include <openssl/ssl.h>
#endif

#include <uv.h>
#include <curl/curl.h>

#include <queue>
#include <cassert>
#include <cstring>


// Check curl library version.
const static bool curl_version_check = []() {
    const auto version = curl_version_info(CURLVERSION_NOW);
    if (version->version_num != LIBCURL_VERSION_NUM) {
        throw std::runtime_error(mbgl::util::sprintf<96>(
            "libcurl version mismatch: headers report %d.%d.%d, but library reports %d.%d.%d",
            (LIBCURL_VERSION_NUM >> 16) & 0xFF, (LIBCURL_VERSION_NUM >> 8) & 0xFF, LIBCURL_VERSION_NUM & 0xFF,
            (version->version_num >> 16) & 0xFF, (version->version_num >> 8) & 0xFF, version->version_num & 0xFF));
    }
    return true;
}();


// This file contains code from http://curl.haxx.se/libcurl/c/multi-uv.html:

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2014, Daniel Stenberg, <daniel@haxx.se>, et al.
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

struct Context {
    const util::ptr<HTTPRequestBaton> baton;
    CURL *handle = nullptr;
    curl_slist *headers = nullptr;

    Context(const util::ptr<HTTPRequestBaton> &baton_) : baton(baton_) {
        assert(baton);
        baton->ptr = this;

        if (!handles.empty()) {
            handle = handles.front();
            handles.pop();
        } else {
            handle = curl_easy_init();
        }
    }

    ~Context() {
        baton->ptr = nullptr;

        if (headers) {
            curl_slist_free_all(headers);
            headers = nullptr;
        }

        CURLMcode error = curl_multi_remove_handle(multi, handle);
        if (error != CURLM_OK) {
            baton->response = std::unique_ptr<Response>(new Response());
            baton->response->code = -1;
            baton->response->message = curl_multi_strerror(error);
        }

        curl_easy_setopt(handle, CURLOPT_PRIVATE, nullptr);
        curl_easy_reset(handle);
        handles.push(handle);
        handle = nullptr;

        if (baton->async) {
            uv_async_send(baton->async);
            baton->async = nullptr;
        }
    }
};

struct Socket {
private:
    uv_poll_t poll_handle;

public:
    const curl_socket_t sockfd = 0;

public:
    Socket(curl_socket_t sockfd_) : sockfd(sockfd_) {
        uv_poll_init_socket(loop, &poll_handle, sockfd);
        poll_handle.data = this;
    }

    void start(int events, uv_poll_cb cb) {
        uv_poll_start(&poll_handle, events, cb);
    }

    void stop() {
        assert(poll_handle.data);
        poll_handle.data = nullptr;
        uv_poll_stop(&poll_handle);
        uv_close((uv_handle_t *)&poll_handle, [](uv_handle_t *handle) {
            delete (Socket *)handle->data;
        });
    }

private:
    // Make the destructor private to ensure that we can only close the Socket
    // with stop(), and disallow manual deletion.
    ~Socket() {
        assert(!poll_handle.data);
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

void check_multi_info() {
    CURLMsg *message = nullptr;
    int pending = 0;

    while ((message = curl_multi_info_read(multi, &pending))) {
        switch (message->msg) {
        case CURLMSG_DONE: {
            Context *context = nullptr;
            curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, (char *)&context);
            assert(context);

            auto baton = context->baton;

            // This request is complete. We are removing the pointer to the CURL easy handle again
            // to prevent this request from getting canceled.
            context->baton->ptr = nullptr;

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

            delete context;
        } break;

        default:
            // This should never happen, because there are no other message types.
            throw std::runtime_error("CURLMSG returned unknown message type");
        }
    }
}

void curl_perform(uv_poll_t *req, int /* status */, int events) {
    int flags = 0;

    uv_timer_stop(&timeout);

    if (events & UV_READABLE) {
        flags |= CURL_CSELECT_IN;
    }
    if (events & UV_WRITABLE) {
        flags |= CURL_CSELECT_OUT;
    }

    Socket *context = (Socket *)req->data;
    int running_handles = 0;
    curl_multi_socket_action(multi, context->sockfd, flags, &running_handles);

    check_multi_info();
}

int handle_socket(CURL * /* handle */, curl_socket_t s, int action, void * /* userp */, void *socketp) {
    Socket *socket = (Socket *)socketp;
    if (!socket && action != CURL_POLL_REMOVE) {
        socket = new Socket(s);
        curl_multi_assign(multi, s, (void *)socket);
    }

    switch (action) {
    case CURL_POLL_IN:
        socket->start(UV_READABLE, curl_perform);
        break;
    case CURL_POLL_OUT:
        socket->start(UV_WRITABLE, curl_perform);
        break;
    case CURL_POLL_REMOVE:
        if (socket) {
            socket->stop();
            curl_multi_assign(multi, s, NULL);
        }
        break;
    default:
        abort();
    }

    return 0;
}

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
void on_timeout(uv_timer_t * /* req */, int /* status */) {
#else
void on_timeout(uv_timer_t * /* req */) {
#endif
    int running_handles;
    CURLMcode error = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    if (error != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(error));
    }

    check_multi_info();
}

void start_timeout(CURLM * /* multi */, long timeout_ms, void * /* userp */) {
    if (timeout_ms <= 0) {
        timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it in a bit */
    }
    uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
}

void thread_init(void *) {
#ifdef __APPLE__
    pthread_setname_np("CURL");
#endif
    thread_id = uv_thread_self();

    if (curl_global_init(CURL_GLOBAL_ALL)) {
        throw std::runtime_error("Could not init cURL");
    }

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
    auto &response = *(std::unique_ptr<Response> *)userp;
    assert(response);
    response->data.append((char *)contents, size * nmemb);
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

    auto &response = *(std::unique_ptr<Response> *)userp;
    assert(response);

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

// This function is called to load the CA bundle
// from http://curl.haxx.se/libcurl/c/cacertinmem.html
#ifdef __ANDROID__
static CURLcode sslctx_function(CURL */*curl*/, void *sslctx, void */*parm*/) {

    int error = 0;
    struct zip *apk = zip_open(mbgl::android::apk_path.c_str(), 0, &error);
    if (apk == nullptr) {
        return CURLE_SSL_CACERT_BADFILE;
    }

    struct zip_file *apk_file = zip_fopen(apk, "assets/ca-bundle.crt", ZIP_FL_NOCASE);
    if (apk_file == nullptr) {
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    struct zip_stat stat;
    if (zip_stat(apk, "assets/ca-bundle.crt", ZIP_FL_NOCASE, &stat) != 0) {
        zip_fclose(apk_file);
        apk_file = nullptr;
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    if (stat.size > std::numeric_limits<int>::max()) {
        zip_fclose(apk_file);
        apk_file = nullptr;
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    const std::unique_ptr<char[]> pem = boost::make_unique<char[]>(stat.size);

    if (zip_fread(apk_file, reinterpret_cast<void *>(pem.get()), stat.size) == -1) {
        zip_fclose(apk_file);
        apk_file = nullptr;
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    // get a pointer to the X509 certificate store (which may be empty!)
    X509_STORE *store = SSL_CTX_get_cert_store((SSL_CTX *)sslctx);
    if (store == nullptr) {
        return CURLE_SSL_CACERT_BADFILE;
    }

    // get a BIO
    BIO *bio = BIO_new_mem_buf(pem.get(), static_cast<int>(stat.size));
    if (bio == nullptr) {
        store = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    // use it to read the PEM formatted certificate from memory into an X509
    // structure that SSL can use
    X509 *cert = nullptr;
    while (PEM_read_bio_X509(bio, &cert, 0, nullptr) != nullptr) {
        if (cert == nullptr) {
            BIO_free(bio);
            bio = nullptr;
            store = nullptr;
            return CURLE_SSL_CACERT_BADFILE;
        }

        // add our certificate to this store
        if (X509_STORE_add_cert(store, cert) == 0) {
            X509_free(cert);
            cert = nullptr;
            BIO_free(bio);
            bio = nullptr;
            store = nullptr;
            return CURLE_SSL_CACERT_BADFILE;
        }

        X509_free(cert);
        cert = nullptr;
    }

    // decrease reference counts
    BIO_free(bio);
    bio = nullptr;

    zip_fclose(apk_file);
    apk_file = nullptr;
    zip_close(apk);
    apk = nullptr;

    // all set to go
    return CURLE_OK;
}
#endif

// This function must run in the CURL thread.
void start_request(void *const ptr) {
    assert(uv_thread_self() == thread_id);

    // The Context object stores information that we need to retain throughout the request, such
    // as the actual CURL easy handle, the baton, and the list of headers. The Context itself is
    // stored in both the CURL easy handle's PRIVATE field, and the baton's `ptr` field.
    auto context = new Context(*(util::ptr<HTTPRequestBaton> *)ptr);
    delete (util::ptr<HTTPRequestBaton> *)ptr;

    if (context->baton->response) {
        if (!context->baton->response->etag.empty()) {
            const std::string header = std::string("If-None-Match: ") + context->baton->response->etag;
            context->headers = curl_slist_append(context->headers, header.c_str());
        } else if (context->baton->response->modified) {
            const std::string time =
                std::string("If-Modified-Since: ") + util::rfc1123(context->baton->response->modified);
            context->headers = curl_slist_append(context->headers, time.c_str());
        }
    }

    if (context->headers) {
        curl_easy_setopt(context->handle, CURLOPT_HTTPHEADER, context->headers);
    }

    if (!context->baton->response) {
        context->baton->response = std::unique_ptr<Response>(new Response());
    }

    // Carry on the shared pointer in the private information of the CURL handle.
    curl_easy_setopt(context->handle, CURLOPT_PRIVATE, context);
#ifndef __ANDROID__
    curl_easy_setopt(context->handle, CURLOPT_CAINFO, "ca-bundle.crt");
#else
    curl_easy_setopt(context->handle, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(context->handle, CURLOPT_SSL_CTX_FUNCTION, sslctx_function);
#endif
    curl_easy_setopt(context->handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(context->handle, CURLOPT_URL, context->baton->path.c_str());
    curl_easy_setopt(context->handle, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(context->handle, CURLOPT_WRITEDATA, &context->baton->response);
    curl_easy_setopt(context->handle, CURLOPT_HEADERFUNCTION, curl_header_cb);
    curl_easy_setopt(context->handle, CURLOPT_HEADERDATA, &context->baton->response);
    curl_easy_setopt(context->handle, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");

    // Start requesting the information.
    curl_multi_add_handle(multi, context->handle);
}

// This function must run in the CURL thread.
void stop_request(void *const ptr) {
    assert(uv_thread_self() == thread_id);
    auto baton = *(util::ptr<HTTPRequestBaton> *)ptr;
    delete (util::ptr<HTTPRequestBaton> *)ptr;
    assert(baton);

    if (baton->async) {
        baton->type = HTTPResponseType::Canceled;

        assert(baton->ptr);

        // We can still stop the request because it is still in progress.
        delete (Context *)baton->ptr;
        assert(!baton->ptr);
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
