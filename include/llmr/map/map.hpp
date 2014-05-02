
#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include <uv.h>

#include <llmr/map/view.hpp>
// #include <llmr/map/tile.hpp>
// #include <llmr/map/tile_data.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/style/style.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/texturepool.hpp>

#include <cstdint>
#include <string>
#include <map>

namespace llmr {

class Source;

class Map : private util::noncopyable {
public:
    explicit Map(View &view);
    ~Map();

    // Start/stop the map render thread
    void start();
    void stop();

    // Runs the map event loop.
    void run();

    // Triggers a lazy rerender: only performs a render when the map is not clean.
    void rerender();

    // Forces a map update: always triggers a rerender.
    void update();

    // Controls buffer swapping.
    bool needsSwap();
    void swapped();

    // Size
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height);

    // Transition
    void cancelTransitions();

    // Position
    void moveBy(double dx, double dy, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void getLonLat(double &lon, double &lat) const;
    void startPanning();
    void stopPanning();
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, double duration = 0);
    void setScale(double scale, double cx = -1, double cy = -1, double duration = 0);
    double getScale() const;
    void setZoom(double zoom, double duration = 0);
    double getZoom() const;
    void setLonLatZoom(double lon, double lat, double zoom, double duration = 0);
    void getLonLatZoom(double &lon, double &lat, double &zoom) const;
    void resetZoom();
    void startScaling();
    void stopScaling();

    // Rotation
    void rotateBy(double sx, double sy, double ex, double ey, double duration = 0);
    void setAngle(double angle, double cx = -1, double cy = -1);
    double getAngle() const;
    void resetNorth();
    void startRotating();
    void stopRotating();

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;

    // TEMPORARY DEBUG API
    void toggleRaster();

public:
    inline const TransformState &getState() const { return state; }
    inline const Style &getStyle() const { return style; }
    inline GlyphAtlas &getGlyphAtlas() { return glyphAtlas; }
    inline uv_loop_t *getLoop() { return loop; }
    inline time getAnimationTime() const { return animationTime; }

private:
    // uv async callbacks
    static void render(uv_async_t *async);
    static void terminate(uv_async_t *async);
    static void delete_async(uv_handle_t *handle);

    // Setup
    void setup();
    void loadStyle(const uint8_t *const data, uint32_t bytes);

    void updateTiles();

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();

    // Unconditionally performs a render with the current map state.
    void render();

private:
    // If cleared, the next time the render thread attempts to render the map, it will *actually*
    // render the map.
    std::atomic_flag is_clean = ATOMIC_FLAG_INIT;

    // If this flag is cleared, the current back buffer is ready for being swapped with the front
    // buffer (i.e. it has rendered data).
    std::atomic_flag is_swapped = ATOMIC_FLAG_INIT;

    // This is cleared once the current front buffer has been presented and the back buffer is
    // ready for rendering.
    std::atomic_flag is_rendered = ATOMIC_FLAG_INIT;

private:
    View &view;
    Transform transform;
    TransformState state;

    Texturepool texturepool;
    Style style;
    GlyphAtlas glyphAtlas;
    Painter painter;

    std::map<std::string, const std::unique_ptr<Source>> sources;

    bool debug = false;
    time animationTime = 0;

private:
    bool async = false;
    uv_loop_t *loop = nullptr;
    uv_thread_t thread;
    uv_async_t *async_terminate = nullptr;
    uv_async_t *async_render = nullptr;
};

}

#endif
