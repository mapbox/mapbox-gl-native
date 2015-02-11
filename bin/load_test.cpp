#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>

#if __APPLE__
#include <mbgl/platform/darwin/log_nslog.hpp>
#else
#include <mbgl/platform/default/log_stderr.hpp>
#endif

#include <uv.h>

#include <cassert>
#include <cstdlib>
#include <iostream>

int main(int, char *[]) {
    const std::string style_path = "styles/styles/bright-v7.json";

    int width = 1024;
    int height = 1024;
    double pixelRatio = 1.0;

    using namespace mbgl;

#if __APPLE__
    Log::Set<NSLogBackend>();
#else
    Log::Set<StderrLogBackend>();
#endif

    mbgl::SQLiteCache cache("cache.sqlite");
    mbgl::DefaultFileSource fileSource(&cache);


    HeadlessView view;
    view.resize(width, height, pixelRatio);

    Map map(view, fileSource, Map::RenderMode::Still);

    // Try to load the token from the environment.
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token) {
        map.setAccessToken(token);
    }

    const std::string style = mbgl::util::read_file(style_path);
    map.setStyleJSON(style, ".");

    static int remaining = 10000;

    srand(time(NULL));

    uv_async_t *async = new uv_async_t;

    static auto render = [&]() {
        const double longitude = ((double(rand()) / RAND_MAX) - 0.5) * 2 * 180;
        const double latitude = ((double(rand()) / RAND_MAX) - 0.5) * 2 * 80;
        const double zoom = (double(rand()) / RAND_MAX) * 9;
        const double bearing = ((double(rand()) / RAND_MAX) - 0.5) * 2 * 180;

        map.setLonLatZoom(longitude, latitude, zoom);
        map.setBearing(bearing);
        fprintf(stderr, "lon: %f, lat: %f, zoom: %f, bearing: %f\n", longitude, latitude, zoom, bearing);
        map.renderStill([async](std::unique_ptr<const StillImage>) {
            uv_async_send(async);
        });
    };

    uv_async_init(uv_default_loop(), async, [](uv_async_t *as, int) {
        if (remaining-- > 0) {
            render();
        } else {
            uv_close(reinterpret_cast<uv_handle_t *>(as), [](uv_handle_t *handle) {
                delete reinterpret_cast<uv_async_t *>(handle);
            });
        }
    });

    render();

    // This loop will terminate once the async was fired.
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
