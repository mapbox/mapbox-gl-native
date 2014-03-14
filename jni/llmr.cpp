#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#include <signal.h>
#include <getopt.h>

#include "settings.hpp"
#include "request.hpp"

std::forward_list<llmr::platform::Request *> requests;

namespace llmr {
namespace platform {

void restart() {
    // if (mapView) {
    //     mapView->dirty = true;
    // }
}

Request *request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback) {
    Request *req = new Request(url, background_function, foreground_callback);
    requests.push_front(req);
    return req;
}

void cancel_request_http(Request *request) {
    for (Request *req : requests) {
        if (req == request) {
            req->cancel();
        }
    }
}

double time() {
    struct timeval Time;
    gettimeofday( &Time, NULL );
    return Time.tv_usec;
}

}
}
