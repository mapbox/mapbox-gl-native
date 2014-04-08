#ifndef LLMR_LINUX_REQUEST
#define LLMR_LINUX_REQUEST

#include <string>
#include <functional>
// #include <atomic>
#include <curl/curl.h>
#include <uv.h>
#include <llmr/util/loop.hpp>



namespace llmr {
namespace platform {

typedef struct curl_context_s curl_context_t;
struct Response;

class Request {
public:
    Request(std::string url, std::function<void(Response *)> fn);

private:
    static void init_thread_cb();
    static void init_thread_run(void *ptr);
    static void async_add_cb(uv_async_t *async, int status);

    static curl_context_t *create_curl_context(curl_socket_t sockfd);
    static void destroy_curl_context(curl_context_t *context);
    static int handle_socket(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp);
    static void curl_perform(uv_poll_t *req, int status, int events);
    static void on_timeout(uv_timer_t *req, int status);
    static void start_timeout(CURLM *multi, long timeout_ms, void *userp);


private:
    const std::string url;
    const std::function<void(Response *)> fn;
    Response *res = nullptr;

private:
    static uv_thread_t thread;
    static uv::loop loop;
    static uv_once_t init_thread;
    static uv_timer_t timeout;
    static uv_async_t async_add;
    static CURLM *curl_handle;

    static std::mutex request_mutex;
    static std::queue<Request *> requests;
};

}
}

#endif
