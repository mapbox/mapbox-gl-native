#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#include <signal.h>
#include <getopt.h>

#include "settings.hpp"
#include "../common/glfw_view.hpp"
#include "request.hpp"



MapView *mapView = nullptr;
std::forward_list<std::shared_ptr<llmr::platform::Request>> requests;




void quit_handler(int s) {
    if (mapView) {
        fprintf(stderr, "waiting for quit...\n");
        glfwSetWindowShouldClose(mapView->window, true);
        llmr::platform::restart();
    } else {
        exit(0);
    }
}





int main(int argc, char *argv[]) {
    int fullscreen_flag = 0;

    const struct option long_options[] = {
        {"fullscreen", no_argument, &fullscreen_flag, 1},
        {0, 0, 0, 0}
    };

    while (true) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "f", long_options, &option_index);
        if (c == -1) break;
    }

    // sigint handling
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = quit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);


    // curl init
    curl_global_init(CURL_GLOBAL_ALL);

    llmr::platform::Request::initialize();


    // main loop
    llmr::Settings_JSON settings;
    mapView = new MapView(settings, fullscreen_flag);
    mapView->init();
    int ret = mapView->run();
    mapView->settings.sync();
    delete mapView;


    llmr::platform::Request::finish();

    curl_global_cleanup();
    return ret;
}

namespace llmr {

void platform::cleanup() {
    bool& dirty = mapView->dirty;
    requests.remove_if([&dirty](std::shared_ptr<llmr::platform::Request> &req) {
        if (req->done) {
            req->foreground_callback();
            dirty = true;
            return true;
        } else {
            return false;
        }
    });
}

void platform::restart() {
    if (mapView) {
        mapView->dirty = true;
    }
}

std::shared_ptr<platform::Request>
platform::request_http(const std::string &url, std::function<void(Response *)> background_function,
                       std::function<void()> foreground_callback) {
    std::shared_ptr<Request> req =
        std::make_shared<Request>(url, background_function, foreground_callback);
    requests.push_front(req);
    return req;
}

// Cancels an HTTP request.
void platform::cancel_request_http(const std::shared_ptr<Request> &req) {
    if (req) {
        req->cancel();
    }
}


}
