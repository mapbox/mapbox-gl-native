#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/style/style.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

namespace po = boost::program_options;

#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    std::string style_path;
    double lat = 0, lon = 0;
    double zoom = 0;
    double bearing = 0;
    double pitch = 0;
    double pixelRatio = 1;

    uint32_t width = 512;
    uint32_t height = 512;
    static std::string output = "out.png";
    std::string cache_file = "cache.sqlite";
    std::string asset_root = ".";
    std::string token;
    bool debug = false;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("style,s", po::value(&style_path)->required()->value_name("json"), "Map stylesheet")
        ("lon,x", po::value(&lon)->value_name("degrees")->default_value(lon), "Longitude")
        ("lat,y", po::value(&lat)->value_name("degrees")->default_value(lat), "Latitude in degrees")
        ("zoom,z", po::value(&zoom)->value_name("number")->default_value(zoom), "Zoom level")
        ("bearing,b", po::value(&bearing)->value_name("degrees")->default_value(bearing), "Bearing")
        ("pitch,p", po::value(&pitch)->value_name("degrees")->default_value(pitch), "Pitch")
        ("width,w", po::value(&width)->value_name("pixels")->default_value(width), "Image width")
        ("height,h", po::value(&height)->value_name("pixels")->default_value(height), "Image height")
        ("ratio,r", po::value(&pixelRatio)->value_name("number")->default_value(pixelRatio), "Image scale factor")
        ("token,t", po::value(&token)->value_name("key")->default_value(token), "Mapbox access token")
        ("debug", po::bool_switch(&debug)->default_value(debug), "Debug mode")
        ("output,o", po::value(&output)->value_name("file")->default_value(output), "Output file name")
        ("cache,d", po::value(&cache_file)->value_name("file")->default_value(cache_file), "Cache database file name")
        ("assets,d", po::value(&asset_root)->value_name("file")->default_value(asset_root), "Directory to which asset:// URLs will resolve")
    ;

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl << desc;
        exit(1);
    }

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fileSource(cache_file, asset_root);

    // Try to load the token from the environment.
    if (!token.size()) {
        const char *token_ptr = getenv("MAPBOX_ACCESS_TOKEN");
        if (token_ptr) {
            token = token_ptr;
        }
    }

    // Set access token if present
    if (token.size()) {
        fileSource.setAccessToken(std::string(token));
    }

    HeadlessBackend backend;
    BackendScope scope { backend };
    OffscreenView view(backend.getContext(), { static_cast<uint32_t>(width * pixelRatio),
                                               static_cast<uint32_t>(height * pixelRatio) });
    ThreadPool threadPool(4);
    Map map(backend, MapObserver::nullObserver(), mbgl::Size { width, height }, pixelRatio, fileSource, threadPool, MapMode::Still);

    if (style_path.find("://") == std::string::npos) {
        style_path = std::string("file://") + style_path;
    }

    map.getStyle().loadURL(style_path);
    map.setLatLngZoom({ lat, lon }, zoom);
    map.setBearing(bearing);
    map.setPitch(pitch);

    if (debug) {
        map.setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus : mbgl::MapDebugOptions::NoDebug);
    }

    map.renderStill(view, [&](std::exception_ptr error) {
        try {
            if (error) {
                std::rethrow_exception(error);
            }
        } catch(std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            exit(1);
        }

        std::ofstream out(output, std::ios::binary);
        out << encodePNG(view.readStillImage());
        out.close();
        loop.stop();
    });

    loop.run();

    return 0;
}
