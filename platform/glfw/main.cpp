#include "glfw_view.hpp"
#include "glfw_renderer_frontend.hpp"
#include "settings_json.hpp"

#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/renderer/renderer.hpp>

#include <args/args.hxx>

#include <csignal>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>

namespace {

GLFWView* view = nullptr;

} // namespace

void quit_handler(int) {
    if (view) {
        mbgl::Log::Info(mbgl::Event::Setup, "waiting for quit...");
        view->setShouldClose();
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    args::ArgumentParser argumentParser("Mapbox GL GLFW example");
    args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {'h', "help"});

    args::Flag fullscreenFlag(argumentParser, "fullscreen", "Toggle fullscreen", {'f', "fullscreen"});
    args::Flag benchmarkFlag(argumentParser, "benchmark", "Toggle benchmark", {'b', "benchmark"});
    args::Flag offlineFlag(argumentParser, "offline", "Toggle offline", {'o', "offline"});

    args::ValueFlag<std::string> styleValue(argumentParser, "URL", "Map stylesheet", {'s', "style"});
    args::ValueFlag<double> lonValue(argumentParser, "degrees", "Longitude", {'x', "lon"});
    args::ValueFlag<double> latValue(argumentParser, "degrees", "Latitude", {'y', "lat"});
    args::ValueFlag<double> zoomValue(argumentParser, "number", "Zoom level", {'z', "zoom"});
    args::ValueFlag<double> bearingValue(argumentParser, "degrees", "Bearing", {'b', "bearing"});
    args::ValueFlag<double> pitchValue(argumentParser, "degrees", "Pitch", {'p', "pitch"});

    try {
        argumentParser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << argumentParser;
        exit(0);
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << argumentParser;
        exit(1);
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << argumentParser;
        exit(2);
    }

    // Load settings
    mbgl::Settings_JSON settings;
    settings.online = !offlineFlag;
    if (lonValue) settings.longitude = args::get(lonValue);
    if (latValue) settings.latitude = args::get(latValue);
    if (zoomValue) settings.zoom = args::get(zoomValue);
    if (bearingValue) settings.bearing = args::get(bearingValue);
    if (pitchValue) settings.pitch = args::get(pitchValue);

    const bool fullscreen = fullscreenFlag ? args::get(fullscreenFlag) : false;
    const bool benchmark = benchmarkFlag ? args::get(benchmarkFlag) : false;
    std::string style = styleValue ? args::get(styleValue) : "";

    // sigint handling
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = quit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

    if (benchmark) {
        mbgl::Log::Info(mbgl::Event::General, "BENCHMARK MODE: Some optimizations are disabled.");
    }

    GLFWView backend(fullscreen, benchmark);
    view = &backend;

    mbgl::DefaultFileSource fileSource("/tmp/mbgl-cache.db", ".");
    if (!settings.online) {
        fileSource.setOnlineStatus(false);
        mbgl::Log::Warning(mbgl::Event::Setup, "Application is offline. Press `O` to toggle online status.");
    }

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        mbgl::Log::Warning(mbgl::Event::Setup, "no access token set. mapbox.com tiles won't work.");
    } else {
        fileSource.setAccessToken(std::string(token));
    }

    mbgl::ThreadPool threadPool(4);
    GLFWRendererFrontend rendererFrontend { std::make_unique<mbgl::Renderer>(backend, view->getPixelRatio(), fileSource, threadPool), backend };
    mbgl::Map map(rendererFrontend, backend, view->getSize(), view->getPixelRatio(), fileSource, threadPool);

    backend.setMap(&map);

    if (!style.empty() && style.find("://") == std::string::npos) {
        style = std::string("file://") + style;
    }

    map.setLatLngZoom(mbgl::LatLng(settings.latitude, settings.longitude), settings.zoom);
    map.setBearing(settings.bearing);
    map.setPitch(settings.pitch);
    map.setDebug(mbgl::MapDebugOptions(settings.debug));

    view->setOnlineStatusCallback([&settings, &fileSource]() {
        settings.online = !settings.online;
        fileSource.setOnlineStatus(settings.online);
        mbgl::Log::Info(mbgl::Event::Setup, "Application is %s. Press `O` to toggle online status.", settings.online ? "online" : "offline");
    });

    view->setChangeStyleCallback([&map] () {
        static uint8_t currentStyleIndex;

        if (++currentStyleIndex == mbgl::util::default_styles::numOrderedStyles) {
            currentStyleIndex = 0;
        }

        mbgl::util::default_styles::DefaultStyle newStyle = mbgl::util::default_styles::orderedStyles[currentStyleIndex];
        map.getStyle().loadURL(newStyle.url);
        view->setWindowTitle(newStyle.name);

        mbgl::Log::Info(mbgl::Event::Setup, "Changed style to: %s", newStyle.name);
    });

    view->setPauseResumeCallback([&fileSource] () {
        static bool isPaused = false;

        if (isPaused) {
            fileSource.resume();
        } else {
            fileSource.pause();
        }

        isPaused = !isPaused;
    });

    // Load style
    if (style.empty()) {
        const char *url = getenv("MAPBOX_STYLE_URL");
        if (url == nullptr) {
            mbgl::util::default_styles::DefaultStyle newStyle = mbgl::util::default_styles::orderedStyles[0];
            style = newStyle.url;
            view->setWindowTitle(newStyle.name);
        } else {
            style = url;
            view->setWindowTitle(url);
        }
    }

    map.getStyle().loadURL(style);

    view->run();

    // Save settings
    mbgl::LatLng latLng = map.getLatLng();
    settings.latitude = latLng.latitude();
    settings.longitude = latLng.longitude();
    settings.zoom = map.getZoom();
    settings.bearing = map.getBearing();
    settings.pitch = map.getPitch();
    settings.debug = mbgl::EnumType(map.getDebug());
    settings.save();
    mbgl::Log::Info(mbgl::Event::General,
                    R"(Exit location: --lat="%f" --lon="%f" --zoom="%f" --bearing "%f")",
                    settings.latitude, settings.longitude, settings.zoom, settings.bearing);

    view = nullptr;
    return 0;
}
