#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/util/util.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/http_header.hpp>

#include <curl/curl.h>

#include <queue>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdio>

static void handleError(CURLMcode code) {
    if (code != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(code));
    }
}

static void handleError(CURLcode code) {
    if (code != CURLE_OK) {
        throw std::runtime_error(std::string("CURL easy error: ") + curl_easy_strerror(code));
    }
}

namespace mbgl {

class HTTPFileSource::Impl {
public:
    Impl();
    ~Impl();

    static int handleSocket(CURL *handle, curl_socket_t s, int action, void *userp, void *socketp);
    static int startTimeout(CURLM *multi, long timeout_ms, void *userp);
    static void onTimeout(HTTPFileSource::Impl *context);

    void perform(curl_socket_t s, util::RunLoop::Event event);
    CURL *getHandle();
    void returnHandle(CURL *handle);
    void checkMultiInfo();

    // Used as the CURL timer function to periodically check for socket updates.
    util::Timer timeout;

    // CURL multi handle that we use to request multiple URLs at the same time, without having to
    // block and spawn threads.
    CURLM *multi = nullptr;

    // CURL share handles are used for sharing session state (e.g.)
    CURLSH *share = nullptr;

    // A queue that we use for storing resuable CURL easy handles to avoid creating and destroying
    // them all the time.
    std::queue<CURL *> handles;
};

class HTTPRequest : public AsyncRequest {
public:
    HTTPRequest(HTTPFileSource::Impl*, Resource, FileSource::Callback);
    ~HTTPRequest() override;

    void handleResult(CURLcode code);

private:
    static size_t headerCallback(char *const buffer, const size_t size, const size_t nmemb, void *userp);
    static size_t writeCallback(void *const contents, const size_t size, const size_t nmemb, void *userp);

    HTTPFileSource::Impl* context = nullptr;
    Resource resource;
    FileSource::Callback callback;

    // Will store the current response.
    std::shared_ptr<std::string> data;
    std::unique_ptr<Response> response;

    optional<std::string> retryAfter;
    optional<std::string> xRateLimitReset;

    CURL *handle = nullptr;
    curl_slist *headers = nullptr;

    char error[CURL_ERROR_SIZE] = { 0 };
};

HTTPFileSource::Impl::Impl() {
    if (curl_global_init(CURL_GLOBAL_ALL)) {
        throw std::runtime_error("Could not init cURL");
    }

    share = curl_share_init();

    multi = curl_multi_init();
    handleError(curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, handleSocket));
    handleError(curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this));
    handleError(curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, startTimeout));
    handleError(curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this));
}

HTTPFileSource::Impl::~Impl() {
    while (!handles.empty()) {
        curl_easy_cleanup(handles.front());
        handles.pop();
    }

    curl_multi_cleanup(multi);
    multi = nullptr;

    curl_share_cleanup(share);
    share = nullptr;

    timeout.stop();
}

CURL *HTTPFileSource::Impl::getHandle() {
    if (!handles.empty()) {
        auto handle = handles.front();
        handles.pop();
        return handle;
    } else {
        return curl_easy_init();
    }
}

void HTTPFileSource::Impl::returnHandle(CURL *handle) {
    curl_easy_reset(handle);
    handles.push(handle);
}

void HTTPFileSource::Impl::checkMultiInfo() {
    CURLMsg *message = nullptr;
    int pending = 0;

    while ((message = curl_multi_info_read(multi, &pending))) {
        switch (message->msg) {
        case CURLMSG_DONE: {
            HTTPRequest *baton = nullptr;
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

void HTTPFileSource::Impl::perform(curl_socket_t s, util::RunLoop::Event events) {
    int flags = 0;

    if (events == util::RunLoop::Event::Read) {
        flags |= CURL_CSELECT_IN;
    }
    if (events == util::RunLoop::Event::Write) {
        flags |= CURL_CSELECT_OUT;
    }


    int running_handles = 0;
    curl_multi_socket_action(multi, s, flags, &running_handles);
    checkMultiInfo();
}

int HTTPFileSource::Impl::handleSocket(CURL * /* handle */, curl_socket_t s, int action, void *userp,
                              void * /* socketp */) {
    assert(userp);
    auto context = reinterpret_cast<Impl *>(userp);

    switch (action) {
    case CURL_POLL_IN: {
        using namespace std::placeholders;
        util::RunLoop::Get()->addWatch(s, util::RunLoop::Event::Read,
                std::bind(&Impl::perform, context, _1, _2));
        break;
    }
    case CURL_POLL_OUT: {
        using namespace std::placeholders;
        util::RunLoop::Get()->addWatch(s, util::RunLoop::Event::Write,
                std::bind(&Impl::perform, context, _1, _2));
        break;
    }
    case CURL_POLL_REMOVE:
        util::RunLoop::Get()->removeWatch(s);
        break;
    default:
        throw std::runtime_error("Unhandled CURL socket action");
    }

    return 0;
}

void HTTPFileSource::Impl::onTimeout(Impl *context) {
    int running_handles;
    CURLMcode error = curl_multi_socket_action(context->multi, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    if (error != CURLM_OK) {
        throw std::runtime_error(std::string("CURL multi error: ") + curl_multi_strerror(error));
    }
    context->checkMultiInfo();
}

int HTTPFileSource::Impl::startTimeout(CURLM * /* multi */, long timeout_ms, void *userp) {
    assert(userp);
    auto context = reinterpret_cast<Impl *>(userp);

    if (timeout_ms < 0) {
        // A timeout of 0 ms means that the timer will invoked in the next loop iteration.
        timeout_ms = 0;
    }

    context->timeout.stop();
    context->timeout.start(mbgl::Milliseconds(timeout_ms), Duration::zero(),
        std::bind(&Impl::onTimeout, context));

    return 0;
}

HTTPRequest::HTTPRequest(HTTPFileSource::Impl* context_, Resource resource_, FileSource::Callback callback_)
    : context(context_),
      resource(std::move(resource_)),
      callback(std::move(callback_)),
      handle(context->getHandle()) {

    // If there's already a response, set the correct etags/modified headers to make sure we are
    // getting a 304 response if possible. This avoids redownloading unchanged data.
    if (resource.priorEtag) {
        const std::string header = std::string("If-None-Match: ") + *resource.priorEtag;
        headers = curl_slist_append(headers, header.c_str());
    } else if (resource.priorModified) {
        const std::string time =
            std::string("If-Modified-Since: ") + util::rfc1123(*resource.priorModified);
        headers = curl_slist_append(headers, time.c_str());
    }

    if (headers) {
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    }

    handleError(curl_easy_setopt(handle, CURLOPT_PRIVATE, this));
    handleError(curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error));
    handleError(curl_easy_setopt(handle, CURLOPT_CAINFO, "ca-bundle.crt"));
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

HTTPRequest::~HTTPRequest() {
    handleError(curl_multi_remove_handle(context->multi, handle));
    context->returnHandle(handle);
    handle = nullptr;

    if (headers) {
        curl_slist_free_all(headers);
        headers = nullptr;
    }
}

// This function is called when we have new data for a request. We just append it to the string
// containing the previous data.
size_t HTTPRequest::writeCallback(void *const contents, const size_t size, const size_t nmemb, void *userp) {
    assert(userp);
    auto impl = reinterpret_cast<HTTPRequest *>(userp);

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

size_t HTTPRequest::headerCallback(char *const buffer, const size_t size, const size_t nmemb, void *userp) {
    assert(userp);
    auto baton = reinterpret_cast<HTTPRequest *>(userp);

    if (!baton->response) {
        baton->response = std::make_unique<Response>();
    }

    const size_t length = size * nmemb;
    size_t begin = std::string::npos;
    if ((begin = headerMatches("last-modified: ", buffer, length)) != std::string::npos) {
        // Always overwrite the modification date; We might already have a value here from the
        // Date header, but this one is more accurate.
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        baton->response->modified = Timestamp{ Seconds(curl_getdate(value.c_str(), nullptr)) };
    } else if ((begin = headerMatches("etag: ", buffer, length)) != std::string::npos) {
        baton->response->etag = std::string(buffer + begin, length - begin - 2); // remove \r\n
    } else if ((begin = headerMatches("cache-control: ", buffer, length)) != std::string::npos) {
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        const auto cc = http::CacheControl::parse(value.c_str());
        baton->response->expires = cc.toTimePoint();
        baton->response->mustRevalidate = cc.mustRevalidate;
    } else if ((begin = headerMatches("expires: ", buffer, length)) != std::string::npos) {
        const std::string value { buffer + begin, length - begin - 2 }; // remove \r\n
        baton->response->expires = Timestamp{ Seconds(curl_getdate(value.c_str(), nullptr)) };
    } else if ((begin = headerMatches("retry-after: ", buffer, length)) != std::string::npos) {
        baton->retryAfter = std::string(buffer + begin, length - begin - 2); // remove \r\n
    } else if ((begin = headerMatches("x-rate-limit-reset: ", buffer, length)) != std::string::npos) {
        baton->xRateLimitReset = std::string(buffer + begin, length - begin - 2); // remove \r\n
    }

    return length;
}

void HTTPRequest::handleResult(CURLcode code) {
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

        if (responseCode == 200) {
            if (data) {
                response->data = std::move(data);
            } else {
                response->data = std::make_shared<std::string>();
            }
        } else if (responseCode == 204 || (responseCode == 404 && resource.kind == Resource::Kind::Tile)) {
            response->noContent = true;
        } else if (responseCode == 304) {
            response->notModified = true;
        } else if (responseCode == 404) {
            response->error =
                std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
        } else if (responseCode == 429) {
            response->error =
                std::make_unique<Error>(Error::Reason::RateLimit, "HTTP status code 429",
                                        http::parseRetryHeaders(retryAfter, xRateLimitReset));
        } else if (responseCode >= 500 && responseCode < 600) {
            response->error =
                std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } +
                                                                   util::toString(responseCode));
        } else {
            response->error =
                std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } +
                                                                  util::toString(responseCode));
        }
    }

    // Calling `callback` may result in deleting `this`. Copy data to temporaries first.
    auto callback_ = callback;
    auto response_ = *response;
    callback_(response_);
}

HTTPFileSource::HTTPFileSource()
    : impl(std::make_unique<Impl>()) {
}

HTTPFileSource::~HTTPFileSource() = default;

std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource& resource, Callback callback) {
    return std::make_unique<HTTPRequest>(impl.get(), resource, callback);
}

uint32_t HTTPFileSource::maximumConcurrentRequests() {
    return 20;
}

} // namespace mbgl
