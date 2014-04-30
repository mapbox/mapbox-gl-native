#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#include <signal.h>
#include <getopt.h>

#include "settings.hpp"
#include "../common/glfw_view.hpp"

MapView *mapView = nullptr;

void quit_handler(int) {
    if (mapView) {
        fprintf(stderr, "waiting for quit...\n");
        glfwSetWindowShouldClose(mapView->window, true);
        glfwPostEmptyEvent();
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

    // main loop
    llmr::Settings_JSON settings;
    mapView = new MapView(settings, fullscreen_flag);
    mapView->init();
    int ret = mapView->run();
    mapView->settings.sync();
    delete mapView;

    return ret;
}
