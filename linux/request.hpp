#ifndef LLMR_LINUX_REQUEST
#define LLMR_LINUX_REQUEST

#include <string>
#include <functional>
#include <curl/curl.h>

namespace llmr {
namespace platform {

struct Response;

class Request {
public:
    Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg);

    static void request(void *);
    static size_t write_callback(void *, size_t, size_t, void *);
    static int progress_callback(void *, double, double, double, double);
    void cancel();

public:
    std::atomic<bool> done;
    std::atomic<bool> cancelled;
    const std::string url;
    const std::function<void(platform::Response&)> background_function;
    const std::function<void()> foreground_callback;
};

}
}

#endif
