#ifndef LLMR_LINUX_REQUEST
#define LLMR_LINUX_REQUEST

#include <string>
#include <functional>
#include <atomic>

namespace llmr {
namespace platform {

struct Response;

class Request {
public:
    Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg);

    static void initialize();
    static void finish();

    void cancel();
    static pthread_key_t key;
    static pthread_once_t key_once;
    std::atomic<bool> done;
    std::atomic<bool> cancelled;
    const std::string url;
    const std::function<void(platform::Response&)> background_function;
    const std::function<void()> foreground_callback;

private:
    static void create_key();
    static void delete_key(void *ptr);
    static void request(void *);

};

}
}

#endif
