#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#include <signal.h>
#include <getopt.h>
#include <fstream>
#include <sstream>

#include "../common/settings_json.hpp"
#include "../common/glfw_view.hpp"
#include "../common/stderr_log.hpp"

GLFWView *view = nullptr;

void quit_handler(int) {
    if (view) {
        fprintf(stderr, "waiting for quit...\n");
        glfwSetWindowShouldClose(view->window, true);
        glfwPostEmptyEvent();
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    llmr::Log::Set<llmr::StderrLogBackend>();

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

    // read default stylesheet from disk
    std::ifstream stylefile("./style.min.js");
    if (!stylefile.good()) {
        fprintf(stderr, "Cannot read style file\n");
        return 1;
    }
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    view = new GLFWView();
    llmr::Map map(*view);

    // Load settings
    llmr::Settings_JSON settings;
    map.setLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    map.setAngle(settings.angle);
    map.setDebug(settings.debug);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        llmr::Log::Warning(llmr::Event::Setup, "no access token set. mapbox.com tiles won't work.");
    } else {
        map.setAccessToken(std::string(token));
    }

    // Load style
    map.setStyleJSON(stylejson.str());

    int ret = view->run();

    // Save settings
    map.getLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    settings.angle = map.getAngle();
    settings.debug = map.getDebug();
    settings.save();

    return ret;
}
