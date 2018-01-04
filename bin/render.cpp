#include <mbgl/map/map.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/default_styles.hpp>

#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/style/style.hpp>

#include <args/args.hxx>

#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    args::ArgumentParser argumentParser("Mapbox GL render tool");
    args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {"help"});

    args::ValueFlag<std::string> tokenValue(argumentParser, "key", "Mapbox access token", {'t', "token"});
    args::ValueFlag<std::string> styleValue(argumentParser, "URL", "Map stylesheet", {'s', "style"});
    args::ValueFlag<std::string> outputValue(argumentParser, "file", "Output file name", {'o', "output"});
    args::ValueFlag<std::string> cacheValue(argumentParser, "file", "Cache database file name", {'c', "cache"});
    args::ValueFlag<std::string> assetsValue(argumentParser, "file", "Directory to which asset:// URLs will resolve", {'a', "assets"});

    args::Flag debugFlag(argumentParser, "debug", "Debug mode", {"debug"});

    args::ValueFlag<double> pixelRatioValue(argumentParser, "number", "Image scale factor", {'r', "ratio"});

    args::ValueFlag<double> zoomValue(argumentParser, "number", "Zoom level", {'z', "zoom"});

    args::ValueFlag<double> lonValue(argumentParser, "degrees", "Longitude", {'x', "lon"});
    args::ValueFlag<double> latValue(argumentParser, "degrees", "Latitude", {'y', "lat"});
    args::ValueFlag<double> bearingValue(argumentParser, "degrees", "Bearing", {'b', "bearing"});
    args::ValueFlag<double> pitchValue(argumentParser, "degrees", "Pitch", {'p', "pitch"});
    args::ValueFlag<uint32_t> widthValue(argumentParser, "pixels", "Image width", {'w', "width"});
    args::ValueFlag<uint32_t> heightValue(argumentParser, "pixels", "Image height", {'h', "height"});

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

    std::string style = styleValue ? args::get(styleValue) : mbgl::util::default_styles::streets.url;
    const double lat = latValue ? args::get(latValue) : 0;
    const double lon = lonValue ? args::get(lonValue) : 0;
    const double zoom = zoomValue ? args::get(zoomValue) : 0;
    const double bearing = bearingValue ? args::get(bearingValue) : 0;
    const double pitch = pitchValue ? args::get(pitchValue) : 0;
    const double pixelRatio = pixelRatioValue ? args::get(pixelRatioValue) : 1;

    const uint32_t width = widthValue ? args::get(widthValue) : 512;
    const uint32_t height = heightValue ? args::get(heightValue) : 512;
    const std::string output = outputValue ? args::get(outputValue) : "out.png";
    const std::string cache_file = cacheValue ? args::get(cacheValue) : "cache.sqlite";
    const std::string asset_root = assetsValue ? args::get(assetsValue) : ".";

    // Try to load the token from the environment.
    const char* tokenEnv = getenv("MAPBOX_ACCESS_TOKEN");
    const std::string token = tokenValue ? args::get(tokenValue) : (tokenEnv ? tokenEnv : std::string());

    const bool debug = debugFlag ? args::get(debugFlag) : false;

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fileSource(cache_file, asset_root);

    // Set access token if present
    if (token.size()) {
        fileSource.setAccessToken(std::string(token));
    }

    ThreadPool threadPool(4);
    HeadlessFrontend frontend({ width, height }, pixelRatio, fileSource, threadPool);
    Map map(frontend, MapObserver::nullObserver(), frontend.getSize(), pixelRatio, fileSource, threadPool, MapMode::Static);

    if (style.find("://") == std::string::npos) {
        style = std::string("file://") + style;
    }

    map.getStyle().loadURL(style);
    map.setLatLngZoom({ lat, lon }, zoom);
    map.setBearing(bearing);
    map.setPitch(pitch);

    if (debug) {
        map.setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus : mbgl::MapDebugOptions::NoDebug);
    }

    try {
        std::ofstream out(output, std::ios::binary);
        out << encodePNG(frontend.render(map));
        out.close();
    } catch(std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        exit(1);
    }

    return 0;
}
