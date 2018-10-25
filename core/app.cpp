#include <mbgl/map/map.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/default_styles.hpp>

#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/style/style.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    std::string style = mbgl::util::default_styles::streets.url;

    const double lat = 0;
    const double lon = 0;
    const double zoom = 0;
    const double bearing = 0;
    const double pitch = 0;
    const double pixelRatio = 1;

    const uint32_t width = 512;
    const uint32_t height = 512;
    const std::string output = "out.png";
    const std::string cache = "cache.sqlite";
    const std::string asset = ".";

    const char* tokenEnv = getenv("MAPBOX_ACCESS_TOKEN");
    const std::string token = tokenEnv ? tokenEnv : std::string();

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fileSource(cache, asset);

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
