#ifndef LLMR_LINUX_REQUEST
#define LLMR_LINUX_REQUEST

#include <string>
#include <functional>
#include <atomic>
#include <curl/curl.h>


namespace llmr {
namespace platform {

struct Response;

class Request {
public:
    Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg);

    static void initialize();
    static void finish();

    void cancel();

private:
    static void create_key();
    static void delete_key(void *ptr);
    static void request(void *);
    static size_t curl_write_callback(void *, size_t, size_t, void *);
    static int curl_progress_callback(void *, double, double, double, double);
    static void curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *);
    static void curl_share_unlock(CURL *, curl_lock_data, void *);

public:
    static pthread_key_t key;
    static pthread_once_t key_once;
    std::atomic<bool> done;
    std::atomic<bool> cancelled;
    const std::string url;
    const std::function<void(platform::Response&)> background_function;
    const std::function<void()> foreground_callback;

private:
    static CURLSH *curl_share;
    static pthread_mutex_t curl_share_mutex;
};

}
}

#endif
