#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/time.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/string.hpp>

#include <curl/curl.h>

#ifdef __ANDROID__
#include <mbgl/android/jni.hpp>
#include <zip.h>
#include <openssl/ssl.h>
#endif

#include <queue>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdio>

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
#define UV_TIMER_PARAMS(timer) uv_timer_t *timer, int
#else
#define UV_TIMER_PARAMS(timer) uv_timer_t *timer
#endif

void handleError(CURLMcode code) {
    if (code != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(code));
    }
}

void handleError(CURLcode code) {
    if (code != CURLE_OK) {
        throw std::runtime_error(std::string("CURL easy error: ") + curl_easy_strerror(code));
    }
}

namespace mbgl {

class HTTPCURLRequest;

class HTTPCURLContext : public HTTPContextBase {
    MBGL_STORE_THREAD(tid)

public:
    explicit HTTPCURLContext(uv_loop_t *loop);
    ~HTTPCURLContext();

    HTTPRequestBase* createRequest(const Resource&,
                               RequestBase::Callback,
                               uv_loop_t*,
                               std::shared_ptr<const Response>) final;

    static int handleSocket(CURL *handle, curl_socket_t s, int action, void *userp, void *socketp);
    static void perform(uv_poll_t *req, int status, int events);
    static int startTimeout(CURLM *multi, long timeout_ms, void *userp);
    static void onTimeout(UV_TIMER_PARAMS(req));

    CURL *getHandle();
    void returnHandle(CURL *handle);
    void checkMultiInfo();

    uv_loop_t *loop = nullptr;

    // Used as the CURL timer function to periodically check for socket updates.
    uv_timer_t *timeout = nullptr;

    // CURL multi handle that we use to request multiple URLs at the same time, without having to
    // block and spawn threads.
    CURLM *multi = nullptr;

    // CURL share handles are used for sharing session state (e.g.)
    CURLSH *share = nullptr;

    // A queue that we use for storing resuable CURL easy handles to avoid creating and destroying
    // them all the time.
    std::queue<CURL *> handles;
};

class HTTPCURLRequest : public HTTPRequestBase {
    MBGL_STORE_THREAD(tid)

public:
    HTTPCURLRequest(HTTPCURLContext*,
                const Resource&,
                Callback,
                uv_loop_t*,
                std::shared_ptr<const Response>);
    ~HTTPCURLRequest();

    void cancel() final;

    void handleResult(CURLcode code);

private:
    static size_t headerCallback(char *const buffer, const size_t size, const size_t nmemb, void *userp);
    static size_t writeCallback(void *const contents, const size_t size, const size_t nmemb, void *userp);

    HTTPCURLContext *context = nullptr;

    // Will store the current response.
    std::shared_ptr<std::string> data;
    std::unique_ptr<Response> response;

    // In case of revalidation requests, this will store the old response.
    const std::shared_ptr<const Response> existingResponse;

    CURL *handle = nullptr;
    curl_slist *headers = nullptr;

    char error[CURL_ERROR_SIZE];
};



struct Socket {
private:
    uv_poll_t poll;

public:
    HTTPCURLContext *context = nullptr;
    const curl_socket_t sockfd = 0;

public:
    Socket(HTTPCURLContext *context_, curl_socket_t sockfd_) : context(context_), sockfd(sockfd_) {
        assert(context);
        uv_poll_init_socket(context->loop, &poll, sockfd);
        poll.data = this;
    }

    void start(int events, uv_poll_cb cb) {
        uv_poll_start(&poll, events, cb);
    }

    void stop() {
        assert(poll.data);
        uv_poll_stop(&poll);
        uv_close((uv_handle_t *)&poll, [](uv_handle_t *handle) {
            assert(handle->data);
            delete reinterpret_cast<Socket *>(handle->data);
        });
    }

private:
    // Make the destructor private to ensure that we can only close the Socket
    // with stop(), and disallow manual deletion.
    ~Socket() = default;
};

// -------------------------------------------------------------------------------------------------

HTTPCURLContext::HTTPCURLContext(uv_loop_t *loop_)
    : HTTPContextBase(),
      loop(loop_) {
    if (curl_global_init(CURL_GLOBAL_ALL)) {
        throw std::runtime_error("Could not init cURL");
    }

    timeout = new uv_timer_t;
    timeout->data = this;
    uv_timer_init(loop, timeout);

    share = curl_share_init();

    multi = curl_multi_init();
    handleError(curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, handleSocket));
    handleError(curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this));
    handleError(curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, startTimeout));
    handleError(curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this));
}

HTTPCURLContext::~HTTPCURLContext() {
    while (!handles.empty()) {
        curl_easy_cleanup(handles.front());
        handles.pop();
    }

    curl_multi_cleanup(multi);
    multi = nullptr;

    curl_share_cleanup(share);
    share = nullptr;

    uv_timer_stop(timeout);
    uv::close(timeout);
}

HTTPRequestBase* HTTPCURLContext::createRequest(const Resource& resource,
                                            RequestBase::Callback callback,
                                            uv_loop_t* loop_,
                                            std::shared_ptr<const Response> response) {
    return new HTTPCURLRequest(this, resource, callback, loop_, response);
}

CURL *HTTPCURLContext::getHandle() {
    if (!handles.empty()) {
        auto handle = handles.front();
        handles.pop();
        return handle;
    } else {
        return curl_easy_init();
    }
}

void HTTPCURLContext::returnHandle(CURL *handle) {
    curl_easy_reset(handle);
    handles.push(handle);
}

void HTTPCURLContext::checkMultiInfo() {
    MBGL_VERIFY_THREAD(tid);
    CURLMsg *message = nullptr;
    int pending = 0;

    while ((message = curl_multi_info_read(multi, &pending))) {
        switch (message->msg) {
        case CURLMSG_DONE: {
            HTTPCURLRequest *baton = nullptr;
            curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, (char *)&baton);
            assert(baton);
            baton->handleResult(message->data.result);
        } break;

        default:
            // This should never happen, because there are no other message types.
            throw std::runtime_error("CURLMsg returned unknown message type");
        }
    }
}

void HTTPCURLContext::perform(uv_poll_t *req, int /* status */, int events) {
    assert(req->data);
    auto socket = reinterpret_cast<Socket *>(req->data);
    auto context = socket->context;
    MBGL_VERIFY_THREAD(context->tid);

    int flags = 0;

    if (events & UV_READABLE) {
        flags |= CURL_CSELECT_IN;
    }
    if (events & UV_WRITABLE) {
        flags |= CURL_CSELECT_OUT;
    }


    int running_handles = 0;
    curl_multi_socket_action(context->multi, socket->sockfd, flags, &running_handles);
    context->checkMultiInfo();
}

int HTTPCURLContext::handleSocket(CURL * /* handle */, curl_socket_t s, int action, void *userp,
                              void *socketp) {
    auto socket = reinterpret_cast<Socket *>(socketp);
    assert(userp);
    auto context = reinterpret_cast<HTTPCURLContext *>(userp);
    MBGL_VERIFY_THREAD(context->tid);

    if (!socket && action != CURL_POLL_REMOVE) {
        socket = new Socket(context, s);
        curl_multi_assign(context->multi, s, (void *)socket);
    }

    switch (action) {
    case CURL_POLL_IN:
        socket->start(UV_READABLE, perform);
        break;
    case CURL_POLL_OUT:
        socket->start(UV_WRITABLE, perform);
        break;
    case CURL_POLL_REMOVE:
        if (socket) {
            socket->stop();
            curl_multi_assign(context->multi, s, nullptr);
        }
        break;
    default:
        throw std::runtime_error("Unhandled CURL socket action");
    }

    return 0;
}

void HTTPCURLContext::onTimeout(UV_TIMER_PARAMS(req)) {
    assert(req->data);
    auto context = reinterpret_cast<HTTPCURLContext *>(req->data);
    MBGL_VERIFY_THREAD(context->tid);
    int running_handles;
    CURLMcode error = curl_multi_socket_action(context->multi, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    if (error != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(error));
    }
    context->checkMultiInfo();
}

int HTTPCURLContext::startTimeout(CURLM * /* multi */, long timeout_ms, void *userp) {
    assert(userp);
    auto context = reinterpret_cast<HTTPCURLContext *>(userp);
    MBGL_VERIFY_THREAD(context->tid);
    if (timeout_ms < 0) {
        // A timeout of 0 ms means that the timer will invoked in the next loop iteration.
        timeout_ms = 0;
    }
    uv_timer_stop(context->timeout);
    uv_timer_start(context->timeout, onTimeout, timeout_ms, 0);
    return 0;
}

// -------------------------------------------------------------------------------------------------

#ifdef __ANDROID__

// This function is called to load the CA bundle
// from http://curl.haxx.se/libcurl/c/cacertinmem.html¯
static CURLcode sslctx_function(CURL * /* curl */, void *sslctx, void * /* parm */) {

    int error = 0;
    struct zip *apk = zip_open(mbgl::android::apkPath.c_str(), 0, &error);
    if (apk == nullptr) {
        return CURLE_SSL_CACERT_BADFILE;
    }

    struct zip_file *apkFile = zip_fopen(apk, "assets/ca-bundle.crt", ZIP_FL_NOCASE);
    if (apkFile == nullptr) {
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    struct zip_stat stat;
    if (zip_stat(apk, "assets/ca-bundle.crt", ZIP_FL_NOCASE, &stat) != 0) {
        zip_fclose(apkFile);
        apkFile = nullptr;
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    if (stat.size > std::numeric_limits<int>::max()) {
        zip_fclose(apkFile);
        apkFile = nullptr;
        zip_close(apk);
        apk = nullptr;
        return CURLE_SSL_CACERT_BADFILE;
    }

    const auto pem = std::make_unique<char[]>(stat.size);

    if (static_cast<zip_uint64_t>(zip_fread(apkFile, reinterpret_cast<void *>(pem.get()), stat.size)) != stat.size) {
        zip_fclose(apkFile);
        apkFile = nullptr;
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

    zip_fclose(apkFile);
    apkFile = nullptr;
    zip_close(apk);
    apk = nullptr;

    // all set to go
    return CURLE_OK;
}
#endif

HTTPCURLRequest::HTTPCURLRequest(HTTPCURLContext* context_, const Resource& resource_, Callback callback_, uv_loop_t*, std::shared_ptr<const Response> response_)
    : HTTPRequestBase(resource_, callback_),
      context(context_),
      existingResponse(response_),
      handle(context->getHandle()) {
    // Zero out the error buffer.
    memset(error, 0, sizeof(error));

    // If there's already a response, set the correct etags/modified headers to make sure we are
    // getting a 304 response if possible. This avoids redownloading unchanged data.
    if (existingResponse) {
        if (!existingResponse->etag.empty()) {
            const std::string header = std::string("If-None-Match: ") + existingResponse->etag;
            headers = curl_slist_append(headers, header.c_str());
        } else if (existingResponse->modified) {
            const std::string time =
                std::string("If-Modified-Since: ") + util::rfc1123(existingResponse->modified);
            headers = curl_slist_append(headers, time.c_str());
        }
    }

    if (headers) {
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    }

    handleError(curl_easy_setopt(handle, CURLOPT_PRIVATE, this));
    handleError(curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error));
#ifdef __ANDROID__
    handleError(curl_easy_setopt(handle, CURLOPT_SSLCERTTYPE, "PEM"));
    handleError(curl_easy_setopt(handle, CURLOPT_SSL_CTX_FUNCTION, sslctx_function));
#else
    handleError(curl_easy_setopt(handle, CURLOPT_CAINFO, "ca-bundle.crt"));
#endif
    handleError(curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1));
    handleError(curl_easy_setopt(handle, CURLOPT_URL, resource.url.c_str()));
    handleError(curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallback));
    handleError(curl_easy_setopt(handle, CURLOPT_WRITEDATA, this));
    handleError(curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback));
    handleError(curl_easy_setopt(handle, CURLOPT_HEADERDATA, this));
#if LIBCURL_VERSION_NUM >= ((7) << 16 | (21) << 8 | 6) // Renamed in 7.21.6
    handleError(curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "gzip, deflate"));
#else
    handleError(curl_easy_setopt(handle, CURLOPT_ENCODING, "gzip, deflate"));
#endif
    handleError(curl_easy_setopt(handle, CURLOPT_USERAGENT, "MapboxGL/1.0"));
    handleError(curl_easy_setopt(handle, CURLOPT_SHARE, context->share));

    // Start requesting the information.
    handleError(curl_multi_add_handle(context->multi, handle));
}

HTTPCURLRequest::~HTTPCURLRequest() {
    MBGL_VERIFY_THREAD(tid);

    handleError(curl_multi_remove_handle(context->multi, handle));
    context->returnHandle(handle);
    handle = nullptr;

    if (headers) {
        curl_slist_free_all(headers);
        headers = nullptr;
    }
}

void HTTPCURLRequest::cancel() {
   delete this;
}

// This function is called when we have new data for a request. We just append it to the string
// containing the previous data.
size_t HTTPCURLRequest::writeCallback(void *const contents, const size_t size, const size_t nmemb, void *userp) {
    assert(userp);
    auto impl = reinterpret_cast<HTTPCURLRequest *>(userp);
    MBGL_VERIFY_THREAD(impl->tid);

    if (!impl->data) {
        impl->data = std::make_shared<std::string>();
    }

    impl->data->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// Compares the beginning of the (non-zero-terminated!) data buffer with the (zero-terminated!)
// header string. If the data buffer contains the header string at the beginning, it returns
// the length of the header string == begin of the value, otherwise it returns npos.
// The comparison of the header is ASCII-case-insensitive.
size_t headerMatches(const char *const header, const char *const buffer, const size_t length) {
    const size_t headerLength = strlen(header);
    if (length < headerLength) {
        return std::string::npos;
    }
    size_t i = 0;
    while (i < length && i < headerLength && std::tolower(buffer[i]) == std::tolower(header[i])) {
        i++;
    }
    return i == headerLength ? i : std::string::npos;
}

size_t HTTPCURLRequest::headerCallback(char *const buffer, const size_t size, const size_t nmemb, void *userp) {
    assert(userp);
    auto baton = reinterpret_cast<HTTPCURLRequest *>(userp);
    MBGL_VERIFY_THREAD(baton->tid);

    if (!baton->response) {
        baton->response = std::make_unique<Response>();
    }

    const size_t length = size * nmemb;
    size_t begin = std::string::npos;
    if ((begin = headerMatches("last-modified: ", buffer, length)) != std::string::npos) {
        // Always overwrite the modification date; We might already have a value here from the
        // Date header, but this one is more accurate.
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        baton->response->modified = curl_getdate(value.c_str(), nullptr);
    } else if ((begin = headerMatches("etag: ", buffer, length)) != std::string::npos) {
        baton->response->etag = { buffer + begin, length - begin - 2 }; // remove \r\n
    } else if ((begin = headerMatches("cache-control: ", buffer, length)) != std::string::npos) {
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        baton->response->expires = parseCacheControl(value.c_str());
    } else if ((begin = headerMatches("expires: ", buffer, length)) != std::string::npos) {
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        baton->response->expires = curl_getdate(value.c_str(), nullptr);
    }

    return length;
}

void HTTPCURLRequest::handleResult(CURLcode code) {
    MBGL_VERIFY_THREAD(tid);

    if (cancelled) {
        // In this case, it doesn't make sense to even process the response even further since
        // the request was canceled anyway.
        delete this;
        return;
    }

    // Make sure a response object exists in case we haven't got any headers or content.
    if (!response) {
        response = std::make_unique<Response>();
    }

    using Error = Response::Error;

    // Add human-readable error code
    if (code != CURLE_OK) {
        switch (code) {
        case CURLE_COULDNT_RESOLVE_PROXY:
        case CURLE_COULDNT_RESOLVE_HOST:
        case CURLE_COULDNT_CONNECT:
        case CURLE_OPERATION_TIMEDOUT:

            response->error = std::make_unique<Error>(
                Error::Reason::Connection, std::string{ curl_easy_strerror(code) } + ": " + error);
            break;

        default:
            response->error = std::make_unique<Error>(
                Error::Reason::Other, std::string{ curl_easy_strerror(code) } + ": " + error);
            break;
        }
    } else {
        long responseCode = 0;
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responseCode);

        // Move over any data we got. We're storing this in a separate object because the Response
        // object defines it as const.
        if (data) {
            response->data = std::move(data);
        } else {
            response->data = std::make_shared<std::string>();
        }

        if (responseCode == 200) {
            // Nothing to do; this is what we want.
        } else if (responseCode == 304) {
            if (existingResponse) {
                // We're going to copy over the existing response's data.
                if (existingResponse->error) {
                    response->error = std::make_unique<Error>(*existingResponse->error);
                }
                response->data = existingResponse->data;
                response->modified = existingResponse->modified;
                // We're not updating `expired`, it was probably set during the request.
                response->etag = existingResponse->etag;
            } else {
                // This is an unsolicited 304 response and should only happen on malfunctioning
                // HTTP servers. It likely doesn't include any data, but we don't have much options.
            }
        } else if (responseCode == 404) {
            response->error =
                std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
        } else if (responseCode >= 500 && responseCode < 600) {
            response->error =
                std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } +
                                                                   std::to_string(responseCode));
        } else {
            response->error =
                std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } +
                                                                  std::to_string(responseCode));
        }
    }

    // Actually return the response.
    notify(std::move(response));
    delete this;
}

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext(uv_loop_t* loop) {
    return std::make_unique<HTTPCURLContext>(loop);
}

} // namespace mbgl
