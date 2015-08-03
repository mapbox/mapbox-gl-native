#include <mbgl/mbgl.hpp>
#include "../platform/default/default_styles.hpp"
#include <mbgl/util/uv.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/default/settings_json.hpp>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <signal.h>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

namespace {

std::unique_ptr<GLFWView> view;

}

void quit_handler(int) {
    if (view) {
        mbgl::Log::Info(mbgl::Event::Setup, "waiting for quit...");
        view->setShouldClose();
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    bool fullscreen = false;
    std::string style;

    const struct option long_options[] = {
        {"fullscreen", no_argument, 0, 'f'},
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
            fullscreen = true;
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

    view = std::make_unique<GLFWView>(fullscreen);

    mbgl::SQLiteCache cache("/tmp/mbgl-cache.db");
    mbgl::DefaultFileSource fileSource(&cache);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        mbgl::Log::Warning(mbgl::Event::Setup, "no access token set. mapbox.com tiles won't work.");
    } else {
        fileSource.setAccessToken(std::string(token));
    }

    mbgl::Map map(*view, fileSource);

    // Load settings
    mbgl::Settings_JSON settings;
    map.setLatLngZoom(mbgl::LatLng(settings.latitude, settings.longitude), settings.zoom);
    map.setBearing(settings.bearing);
    map.setDebug(settings.debug);

    view->setChangeStyleCallback([&map] () {
        static uint8_t currentStyleIndex;

        if (++currentStyleIndex == mbgl::util::defaultStyles.size()) {
            currentStyleIndex = 0;
        }

        const auto& newStyle = mbgl::util::defaultStyles[currentStyleIndex];
        map.setStyleURL(newStyle.first);
        view->setWindowTitle(newStyle.second);

        mbgl::Log::Info(mbgl::Event::Setup, std::string("Changed style to: ") + newStyle.first);
    });

    // Load style
    if (style.empty()) {
        const auto& newStyle = mbgl::util::defaultStyles.front();
        style = newStyle.first;
        view->setWindowTitle(newStyle.second);
    }

    map.setStyleURL(style);

    view->run();

    // Save settings
    mbgl::LatLng latLng = map.getLatLng();
    settings.latitude = latLng.latitude;
    settings.longitude = latLng.longitude;
    settings.zoom = map.getZoom();
    settings.bearing = map.getBearing();
    settings.debug = map.getDebug();
    settings.save();

    return 0;
}
