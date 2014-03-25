#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#include <signal.h>
#include <getopt.h>

#include "../common/map_view.cpp"
#include "settings.hpp"
#include "request.hpp"



MapView *mapView = nullptr;
std::forward_list<llmr::platform::Request *> requests;




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
namespace platform {

void cleanup() {
    bool& dirty = mapView->dirty;
    requests.remove_if([&dirty](llmr::platform::Request * req) {
        if (req->done) {
            req->foreground_callback();
            delete req;
            dirty = true;
            return true;
        } else {
            return false;
        }
    });
}

void restart() {
    if (mapView) {
        mapView->dirty = true;
    }
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

}
}
