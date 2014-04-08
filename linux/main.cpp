#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#include <signal.h>
#include <getopt.h>

#include "../common/glfw_view.cpp"
#include "settings.hpp"
#include "request.hpp"

#include <curl/curl.h>

GLFWView *mapView = nullptr;

void quit_handler(int s) {
    if (mapView) {
        fprintf(stderr, "waiting for quit...\n");
        mapView->close();
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int fullscreen_flag = 0;

    const struct option long_options[] = {{"fullscreen", no_argument, &fullscreen_flag, 1},
                                          {0, 0, 0, 0}};

    while (true) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "f", long_options, &option_index);
        if (c == -1)
            break;
    }

    // sigint handling
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = quit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // curl init
    curl_global_init(CURL_GLOBAL_ALL);

    // main loop
    llmr::Settings_JSON settings;
    llmr::Map map(settings);
    llmr::GLFWView view(map);
    mapView = &view;
    view.show();
    settings.sync();

    curl_global_cleanup();
    return 0;
}
