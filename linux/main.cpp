#include <mbgl/mbgl.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/default/settings_json.hpp>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/platform/default/log_stderr.hpp>
#include <mbgl/storage/caching_http_file_source.hpp>

#include <signal.h>
#include <getopt.h>
#include <fstream>
#include <sstream>


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
    mbgl::Log::Set<mbgl::StderrLogBackend>();

    int fullscreen_flag = 0;
    std::string style;

    const struct option long_options[] = {
        {"fullscreen", no_argument, &fullscreen_flag, 'f'},
        {"style", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };

    while (true) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, "fs:", long_options, &option_index);
        if (opt == -1) break;
        switch (opt)
        {
        case 0:
            if (long_options[option_index].flag != 0)
                break;
        case 'f':
            // handle fullscreen_flag
            break;
        case 's':
            style = std::string("asset://") + std::string(optarg);
        default:
            break;
        }

    }

    // sigint handling
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = quit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    view = new GLFWView();
    mbgl::CachingHTTPFileSource fileSource(mbgl::platform::defaultCacheDatabase());
    mbgl::Map map(*view, fileSource);

    // Load settings
    mbgl::Settings_JSON settings;
    map.setLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    map.setBearing(settings.bearing);
    map.setDebug(settings.debug);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        mbgl::Log::Warning(mbgl::Event::Setup, "no access token set. mapbox.com tiles won't work.");
    } else {
        fileSource.setAccessToken(std::string(token));
    }

    // Load style
    if (style.empty())
        style = std::string("asset://") + std::string("styles/bright-v6.json");

    map.setStyleURL(style);

    int ret = view->run();

    // Save settings
    map.getLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    settings.bearing = map.getBearing();
    settings.debug = map.getDebug();
    settings.save();

    return ret;
}
